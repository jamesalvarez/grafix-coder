#include "GPMatrixFiles.h"

using namespace GPMatrixFiles;

namespace GPMatrixFiles {
    bool readFile(mat &matrix, QString fileName);
    bool saveFile(mat &matrix, QString fileName, QString headerString);
}


bool GPMatrixFiles::readFile(mat &matrix, QString fileName)
{
     std::string filename = fileName.toStdString();

     double* squarematrix;
     unsigned char* buffer_;
     unsigned int buffer_size_;

     std::ifstream stream(filename.c_str(),std::ios::binary);

     if (stream) {

         stream.seekg (0,std::ios::end);
         buffer_size_ = static_cast<std::size_t>(stream.tellg());
         stream.seekg (0,std::ios::beg);

         if (0 != buffer_size_) {
             try {buffer_ = new unsigned char[buffer_size_];}
             catch (...){throw (std::runtime_error("Out of memory when loading: " + filename)); return false;}
             try {squarematrix = new double[buffer_size_/2];} //should have enough room
             catch (...){delete[] buffer_;throw (std::runtime_error("Out of memory when loading: " + filename)); return false;}
             unsigned int row = 0;
             unsigned int items = 0;

             try {
             stream.read(reinterpret_cast<char*>(buffer_),static_cast<std::streamsize>(buffer_size_));
             qDebug() << QString::fromStdString(filename) + " -Loaded";

             bool found_a_number = false;
             //parse
             for (unsigned int pos = 0; pos<buffer_size_; ++pos) {
                 switch (buffer_[pos]) {
                 case ',':
                     found_a_number=false;
                     break;
                 case '\n':
                     found_a_number=false;
                     ++row;
                     break;
                 case '0': case '1':case '2': case '3': case '4':
                 case '5': case '6': case '7': case '8': case '9':
                 case '-': case '+':
                     if (!found_a_number) {  //start of a string
                         unsigned int move_on=0;
                         squarematrix[items++] = GPGeneralStaticLibrary::string_to_double_fast(buffer_+pos,move_on);
                         pos += move_on;
                         found_a_number=true;
                     }
                     break;
                 default:  //whitespace
                     break;
                 }
             }

             unsigned int n_cols = items / row;
             matrix = mat(squarematrix,n_cols,row).t();
                 } catch (...) {
                     delete[] buffer_;
                     delete[] squarematrix;
                     buffer_ = 0;
                     squarematrix = 0;
                     qDebug() << "Error loading file";
                     throw (std::runtime_error("Error parsing CSV: " + filename));
                     return false;
                 }
             delete[] buffer_;
             delete[] squarematrix;
             buffer_ = 0;
             squarematrix = 0;
         }
         stream.close();
         return true;
     }
     throw (std::runtime_error("Error loading: " + filename));
     return false;
}

bool GPMatrixFiles::readFileSafe(mat &matrix, QString fileName) {
    try {
        return readFile(matrix,fileName);
    } catch(...) {
        return false;
    }
}

int GPMatrixFiles::getFileColumnCount(QString fullpath)
{
    std::string filename = fullpath.toStdString();

    unsigned char* buffer_;
    unsigned int buffer_size_;
    int n_cols = 0;

    std::ifstream stream(filename.c_str(),std::ios::binary);

    if (stream) {

        stream.seekg (0,std::ios::end);
        buffer_size_ = static_cast<std::size_t>(stream.tellg());
        stream.seekg (0,std::ios::beg);

        if (0 != buffer_size_) {
            try {buffer_ = new unsigned char[buffer_size_];}
            catch (...){return 0;}

            try {
            stream.read(reinterpret_cast<char*>(buffer_),static_cast<std::streamsize>(buffer_size_));
            qDebug() << QString::fromStdString(filename) + " -Loaded";

            bool found_a_number = false;
            bool found_a_row = false;
            unsigned int pos = 0;
            //parse
            while (!found_a_row && pos<buffer_size_)
            {
                switch (buffer_[pos])
                {
                case ',':
                    n_cols++;
                    found_a_number=false;
                    break;
                case '\n':
                    n_cols++;
                    found_a_row = true;
                    found_a_number=false;
                    break;
                case '0': case '1':case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                case '-': case '+':
                    if (!found_a_number) {  //start of a string
                        unsigned int move_on=0;
                        GPGeneralStaticLibrary::string_to_double_fast(buffer_+pos,move_on);
                        pos += move_on;
                        found_a_number=true;
                    }
                    break;
                default:  //whitespace
                    break;
                }
                ++pos;
            }

            } catch (...) {
                delete[] buffer_;
                buffer_ = 0;
                qDebug() << "Error loading file";
                return 0;
            }
            delete[] buffer_;
            buffer_ = 0;
        }
        stream.close();
        return n_cols;
    }
    return 0;
}

bool GPMatrixFiles::saveFile(mat &matrix, QString fileName, QString headerString)
{
    std::string filename = fileName.toStdString();
    std::ofstream stream(filename.c_str(),std::ios::binary);

    stream << std::fixed;

    if (headerString.length() > 0) {
        stream << headerString.toStdString() << endl;
    }


    uword nRows = matrix.n_rows;
    uword nCols = matrix.n_cols;
    if (!stream) return false;
    for (uword row = 0; row < nRows; ++row)
    {
        for (uword col = 0; col < (nCols-1); ++col)
        {
            stream << matrix.at(row,col);
            stream << ',';
        }
        stream << matrix.at(row,nCols-1);
        stream << endl;
    }
    stream.close();
    return true;
}

bool GPMatrixFiles::saveFileSafe(mat &matrix, QString fileName, QString headerString) {
    try {
        qDebug() << "Saving matrix: " << fileName;
        return saveFile(matrix, fileName, headerString);
    } catch(...) {
        qDebug() << "Saving unsuccessful";
        return false;
    }
}


bool GPMatrixFiles::exportFile(mat &roughM, mat &smoothM, mat &fixAllM, QString filename, GrafixSettingsLoader &settingsLoader) {

    if (filename == "") {
        return false;
    }

    // Calculate saccades
    mat saccadesM;
    GPMatrixFunctions::fncCalculateSaccades(saccadesM, fixAllM, smoothM, settingsLoader);

    // Collect data to export in matrix
    mat exportM = roughM.col(0);

    QString headerString = "timestamp,";

    if (settingsLoader.LoadSetting(Consts::SETTING_EXPORT_LEFT_X_ROUGH).toBool()){    // X left rough
        exportM = join_rows(exportM, roughM.col(2));
        headerString += "rough_left_x,";
    }
    if (settingsLoader.LoadSetting(Consts::SETTING_EXPORT_LEFT_Y_ROUGH).toBool()){    // Y left rough
        exportM = join_rows(exportM, roughM.col(3));
        headerString += "rough_left_y,";
    }
    if (settingsLoader.LoadSetting(Consts::SETTING_EXPORT_RIGHT_X_ROUGH).toBool()){    // x right rough
        exportM = join_rows(exportM, roughM.col(4));
        headerString += "rough_right_x,";
    }
    if (settingsLoader.LoadSetting(Consts::SETTING_EXPORT_RIGHT_Y_ROUGH).toBool()){    // Y right rough
        exportM = join_rows(exportM, roughM.col(5));
        headerString += "rough_right_y,";
    }
    if (settingsLoader.LoadSetting(Consts::SETTING_EXPORT_LEFT_PUPIL).toBool()){    // Pupil dilation left
        exportM = join_rows(exportM, roughM.col(6));
        headerString += "pupil_left,";
    }
    if (settingsLoader.LoadSetting(Consts::SETTING_EXPORT_RIGHT_PUPIL).toBool()){    // Pupil dilation right
        exportM = join_rows(exportM, roughM.col(7));
        headerString += "pupil_right,";
    }
    if (settingsLoader.LoadSetting(Consts::SETTING_EXPORT_X_SMOOTH).toBool()){    // Flitered X
        if (smoothM.n_rows == exportM.n_rows) {
            exportM = join_rows(exportM, smoothM.col(2));
        } else {
            exportM.insert_cols(exportM.n_cols, 1, true);
        }
        headerString += "smooth_x,";
    }
    if (settingsLoader.LoadSetting(Consts::SETTING_EXPORT_Y_SMOOTH).toBool()){    // Flitered Y
        if (smoothM.n_rows == exportM.n_rows) {
            exportM = join_rows(exportM, smoothM.col(3));
        } else {
            exportM.insert_cols(exportM.n_cols, 1, true);
        }
        headerString += "smooth_y,";
    }
    if (settingsLoader.LoadSetting(Consts::SETTING_EXPORT_VELOCITY).toBool()) {
        if (smoothM.n_rows == exportM.n_rows && smoothM.n_cols >= 4) {
            exportM = join_rows(exportM, smoothM.col(4));
        } else {
            exportM.insert_cols(exportM.n_cols, 1, true);
        }
        headerString += "smooth_velocity,";
    }

    if (settingsLoader.LoadSetting(Consts::SETTING_EXPORT_FIXATION_NUMBER).toBool()){    // Fixation number!

        int new_col_index = exportM.n_cols;
        exportM.insert_cols(new_col_index,1,true);

        // Calculate fixation number
        for (uword i = 0; i < fixAllM.n_rows ; i ++) {
            exportM.rows(fixAllM(i,FIXCOL_START), fixAllM(i,FIXCOL_END)).col(new_col_index).fill(i+1);
        }
        headerString += "fixation_number,";
    }

    if (settingsLoader.LoadSetting(Consts::SETTING_EXPORT_FIXATION_DURATION).toBool()){    // Fixation duration!
        int new_col_index = exportM.n_cols;
        exportM.insert_cols(new_col_index,1,true);

        for (uword i = 0; i < fixAllM.n_rows ; i ++) {
            exportM.rows(fixAllM(i,FIXCOL_START), fixAllM(i,FIXCOL_END)).col(new_col_index).fill(fixAllM(i,FIXCOL_DURATION));
        }
        headerString += "fixation_duration,";
    }

    if (settingsLoader.LoadSetting(Consts::SETTING_EXPORT_FIXATION_X_AVERAGE).toBool()){    // Average X
        int new_col_index = exportM.n_cols;
        exportM.insert_cols(new_col_index,1,true);

        for (uword i = 0; i < fixAllM.n_rows ; i ++) {
            exportM.rows(fixAllM(i,FIXCOL_START), fixAllM(i,FIXCOL_END)).col(new_col_index).fill(fixAllM(i,FIXCOL_AVERAGEX));
        }
        headerString += "fixation_x,";
    }

    if (settingsLoader.LoadSetting(Consts::SETTING_EXPORT_FIXATION_Y_AVERAGE).toBool()){    // Average y
        int new_col_index = exportM.n_cols;
        exportM.insert_cols(new_col_index,1,true);

        for (uword i = 0; i < fixAllM.n_rows ; i ++) {
            exportM.rows(fixAllM(i,FIXCOL_START), fixAllM(i,FIXCOL_END)).col(new_col_index).fill(fixAllM(i,FIXCOL_AVERAGEY));
        }
        headerString += "fixation_y,";
    }

    if (settingsLoader.LoadSetting(Consts::SETTING_EXPORT_FIXATION_RMS).toBool()){    // RMS
        int new_col_index = exportM.n_cols;
        exportM.insert_cols(new_col_index,1,true);

        for (uword i = 0; i < fixAllM.n_rows ; i ++) {
            exportM.rows(fixAllM(i,FIXCOL_START), fixAllM(i,FIXCOL_END)).col(new_col_index).fill(fixAllM(i,FIXCOL_RMS));
        }
        headerString += "fixation_rmd,";
    }

    if (settingsLoader.LoadSetting(Consts::SETTING_EXPORT_FIXATION_IS_SMOOTH_PURSUIT).toBool()){    // Is smooth pursuit?
        int new_col_index = exportM.n_cols;
        exportM.insert_cols(new_col_index,1,true);

        for (uword i = 0; i < fixAllM.n_rows ; i ++) {
            exportM.rows(fixAllM(i,FIXCOL_START), fixAllM(i,FIXCOL_END)).col(new_col_index).fill(fixAllM(i,FIXCOL_SMOOTH_PURSUIT));
        }
        headerString += "fixation_smooth_pursuit,";
    }

    if (settingsLoader.LoadSetting(Consts::SETTING_EXPORT_SACCADE_NUMBER).toBool()){    // Saccade number!
        int new_col_index = exportM.n_cols;
        exportM.insert_cols(new_col_index,1,true);

        for (uword i = 0; i < saccadesM.n_rows ; i ++) {
            exportM.rows(saccadesM(i,0), saccadesM(i,1)).col(new_col_index).fill(i + 1);
        }
        headerString += "sacade_number,";
    }

    if (settingsLoader.LoadSetting(Consts::SETTING_EXPORT_SACCADE_DURATION).toBool()){
        int new_col_index = exportM.n_cols;
        exportM.insert_cols(new_col_index,1,true);

        for (uword i = 0; i < saccadesM.n_rows ; i ++) {
            exportM.rows(saccadesM(i,0), saccadesM(i,1)).col(new_col_index).fill(saccadesM(i,2));
        }
        headerString += "saccade_duration,";
    }

    if (settingsLoader.LoadSetting(Consts::SETTING_EXPORT_SACCADE_DISTANCE).toBool()){
        int new_col_index = exportM.n_cols;
        exportM.insert_cols(new_col_index,1,true);

        for (uword i = 0; i < saccadesM.n_rows ; i ++) {
            exportM.rows(saccadesM(i,0), saccadesM(i,1)).col(new_col_index).fill(saccadesM(i,3));
        }
        headerString += "saccade_distance,";
    }

    if (settingsLoader.LoadSetting(Consts::SETTING_EXPORT_SACCADE_VELOCITY_AVERAGE).toBool()){
        int new_col_index = exportM.n_cols;
        exportM.insert_cols(new_col_index,1,true);

        for (uword i = 0; i < saccadesM.n_rows ; i ++) {
            exportM.rows(saccadesM(i,0), saccadesM(i,1)).col(new_col_index).fill(saccadesM(i,4));
        }
        headerString += "saccade_velocity_average,";
    }

    if (settingsLoader.LoadSetting(Consts::SETTING_EXPORT_SACCADE_VELOCITY_PEAK).toBool()){    // Saccade Amplitude
        int new_col_index = exportM.n_cols;
        exportM.insert_cols(new_col_index,1,true);

        for (uword i = 0; i < saccadesM.n_rows ; i ++) {
            exportM.rows(saccadesM(i,0), saccadesM(i,1)).col(new_col_index).fill(saccadesM(i,5));
        }
        headerString += "saccade_velocity_peak,";
    }

    // Remove last comma
    if (headerString.length() > 0) {
        headerString = headerString.left(headerString.length() - 1);
    }

    return GPMatrixFiles::saveFileSafe(exportM, filename, headerString);
}

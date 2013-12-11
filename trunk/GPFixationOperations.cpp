// IN this class we include all the methods for create, delete, and merge fixations.

#include "GPFixationOperations.h"

mat GPFixationOperations::fncCreateFixation(mat fixAllM, mat roughM, int hz, int secsSegment,int segment, int from, int to,int  expWidth, int expHeight, double degPerPixel){

    if ((uword)to < roughM.n_rows && (uword)from < roughM.n_rows){ // Only if we are creating the fixation in the right place
        int startIndexSegment = ((segment-1) * secsSegment * hz);

        int auxIndex = 0;
        if (fixAllM.n_rows > 0){
            uvec fixIndex =  arma::find(fixAllM.col(1) >= startIndexSegment);
            if (!fixIndex.empty()){
                auxIndex = fixIndex(0);
            }
        }

        int startIndex;
        int fromSegment = floor((double)(from / (secsSegment * hz)));
        if (fromSegment < segment - 1 || auxIndex <= 1){  // Try to optimize it a bit. Otherwise it would run the whole array everytime.
            startIndex = 1;
        }else{
            startIndex = auxIndex-1;
        }
        int firstElem = -1;


        if (fixAllM.n_rows == 1){
            if (fixAllM(0,0) > to ){
                firstElem = 0;
            }else if (fixAllM(0,1) < from ){
                firstElem = 1;
            }
        }else if (fixAllM.n_rows != 0){
            for (uword i = (uword)startIndex; i < fixAllM.n_rows  ; i++){
                if (i == 1 && fixAllM(i-1,0)> to ){
                    firstElem = 0;
                }else if (fixAllM(i-1,1) < from && fixAllM(i,0) > to ){ // The space is free
                    firstElem = i;
                }else if(i ==  (fixAllM.n_rows -1 ) && fixAllM(i,1) < from ){
                    firstElem = fixAllM.n_rows ;
                }else if (fixAllM(i-1,0) > to){ // Stop execution
                    break;
                }

            }
        }

        if (firstElem == -1 && fixAllM.n_rows != 0){ // SHOW DIALOG INDICATING THAT THE SPACE IS NOT FREE
           // cout << "THE SPACE IS NOT FREE" << endl;
        }else{ // Modify the matrix
            // Create row: newRow = [firstFix, lastFix, duration, averageX, averageY, variance, smooth pursuit, PupilDilation];
            double dur = ((roughM(to,0) - roughM(0,0))/ 1000 ) - ((roughM(from,0) - roughM(0,0))/ 1000 );

            // Only use the points where eyes were detected:
            mat roughCutM;
            if (roughM.n_cols == 8){
                roughCutM = roughM.submat(from,2,to,7);
                GPMatrixFunctions::fncRemoveUndetectedValuesRough(&roughCutM);
            }else{
                 roughCutM = roughM.submat(from,2,to,5);
                 GPMatrixFunctions::fncRemoveUndetectedValuesRough(&roughCutM);
            }

            double averageX = mean((roughCutM.col(0) + roughCutM.col(2))/2);
            double averageY = mean((roughCutM.col(1) + roughCutM.col(3))/2);
            double variance = GPMatrixFunctions::fncCalculateRMSRough(&roughCutM, expWidth, expHeight, degPerPixel);
            double pupilDilation = 0;
            if (roughCutM.n_cols == 6){
                pupilDilation = (mean(roughCutM.col(4)) + mean(roughCutM.col(5)))/2;
            }

            mat row;
            row << from << to << dur << averageX << averageY << variance << 0 << pupilDilation <<  endr ;

            // Insert row.
            if (fixAllM.n_rows == 0){
                fixAllM = row;
            }else if (firstElem == 0){
                 fixAllM = join_cols(row,fixAllM);
            }else if(firstElem == (int)fixAllM.n_rows){
                fixAllM = join_cols(fixAllM, row);
            }else{
                mat F1 = fixAllM( span(0, firstElem-1), span(0, fixAllM.n_cols-1) );
                mat F2 = fixAllM(span(firstElem,fixAllM.n_rows-1),span(0, fixAllM.n_cols-1) );
                mat aux = join_cols(F1,row);
                fixAllM = join_cols(aux, F2);
            }
        }

    }

    return fixAllM;
}

mat GPFixationOperations::fncDeleteFixations(mat fixAllM, int hz, int secsSegment,int segment, int from, int to){

    if (from >= 0 && fixAllM.n_rows >0) {
        int startIndexSegment = ((segment-1) * secsSegment * hz);
        int auxIndex = 0;
        uvec fixIndex =  arma::find(fixAllM.col(1) >= startIndexSegment);
        if (!fixIndex.empty()){
            auxIndex = fixIndex(0);
        }

        int startIndex;
        int fromSegment = floor((double)(from / (secsSegment * hz)));
        if (fromSegment < segment - 1){  // Try to optimize it a bit. Otherwise it would run the whole array everytime.
            startIndex = 0;
        }else{
            startIndex = auxIndex;
        }
        int firstElem = -2;
        int lastElem = -2;

        for (uword i = (uword)startIndex; i < fixAllM.n_rows  ; i++){
             if  ( firstElem == -2 && ((fixAllM(i,0) >= from && fixAllM(i,0) <=  to ) ||(fixAllM(i,0) <= from && fixAllM(i,1) >= from ))){
                 // We found the first elem to delete
                 firstElem = i-1;
                 lastElem = i+ 1;
             }else if (firstElem != -2 && ((fixAllM(i,0) >= from && fixAllM(i,0) <=  to ) ||(fixAllM(i,0) <= from && fixAllM(i,1) >= from ))){
                // THere are still elements to delete
                 lastElem = i + 1;
             }else if(firstElem != -2 && ((fixAllM(i,0) >= from && fixAllM(i,1) >= from))){
                 break;
             }
        }
        if ( fixAllM.n_rows == 1 || (firstElem == -1 && lastElem == (int)fixAllM.n_rows)){
            fixAllM.zeros(0,0);
        }else if (lastElem == (int)fixAllM.n_rows){ // the last element
            fixAllM = fixAllM( span(0, firstElem), span(0, fixAllM.n_cols-1) );
        }else if(firstElem >= 0 && firstElem <= lastElem){ // If the values are correct DELETE FIXATIONS
            mat F1 = fixAllM( span(0, firstElem), span(0, fixAllM.n_cols-1) );
            mat F2 = fixAllM(span(lastElem,fixAllM.n_rows-1),span(0, fixAllM.n_cols-1) );
            fixAllM = join_cols(F1,F2);
        }else if (firstElem == -1 && lastElem >= 1){ // It is the very first fixation
            fixAllM = fixAllM( span(lastElem, fixAllM.n_rows-1), span(0, fixAllM.n_cols-1) );
        }
    }
    return fixAllM;


}


mat GPFixationOperations::fncSmoothPursuitFixation(mat fixAllM, int hz, int secsSegment,int segment, int from, int to){

    // Identify the selected fixations and flag them as smooth pursuit
    if (from >= 0) {
        int startIndexSegment = ((segment-1) * secsSegment * hz);
        int auxIndex = 0;
        uvec fixIndex =  arma::find(fixAllM.col(1) >= startIndexSegment);
        if (!fixIndex.empty()){
            auxIndex = fixIndex(0);
        }

        int startIndex;
        int fromSegment = floor((double)(from / (secsSegment * hz)));
        if (fromSegment < segment - 1){  // Try to optimize it a bit. Otherwise it would run the whole array everytime.
            startIndex = 0;
        }else{
            startIndex = auxIndex;
        }
        int firstElem = -2;

        for (uword i = (uword)startIndex; i < fixAllM.n_rows  ; i++){
             if  ( firstElem == -2 && ((fixAllM(i,0) >= from && fixAllM(i,0) <=  to ) ||(fixAllM(i,0) <= from && fixAllM(i,1) >= from ))){
                 // We found the first elem to delete
                 if (fixAllM(i,5) == 0)
                    fixAllM(i,5) = Consts::SMOOTHP_YES;
                 else
                     fixAllM(i,5) = Consts::SMOOTHP_NO;
             }else if (firstElem != -2 && ((fixAllM(i,0) >= from && fixAllM(i,0) <=  to ) ||(fixAllM(i,0) <= from && fixAllM(i,1) >= from ))){
                // THere are still elements to delete
                 if (fixAllM(i,5) == 0)
                    fixAllM(i,5) = Consts::SMOOTHP_YES;
                 else
                     fixAllM(i,5) = Consts::SMOOTHP_NO;
             }else if(firstElem != -2 && ((fixAllM(i,0) >= from && fixAllM(i,1) >= from))){
                 break;
             }
        }
    }

    return fixAllM;


}

mat GPFixationOperations::fncMergeFixations(mat fixAllM, mat roughM, int hz, int secsSegment,int segment, int from, int to, int expWidth, int expHeight, double degPerPixel){
    if (from >= 0) {
        int startIndexSegment = ((segment-1) * secsSegment * hz);
        int auxIndex = 0;
        uvec fixIndex =  arma::find(fixAllM.col(1) >= startIndexSegment);
        if (!fixIndex.empty()){
            auxIndex = fixIndex(0);
        }

        int startIndex, stopIndex;
        int fromSegment = floor((double)(from / (secsSegment * hz)));
        if (fromSegment < segment - 1){  // Try to optimize it a bit. Otherwise it would run the whole array everytime.
            startIndex = 0;
        }else{
            startIndex = auxIndex;
        }
        int firstElem = -2;
        int lastElem = -2;

        for (uword i = (uword)startIndex; i < fixAllM.n_rows  ; i++){

             if  ( firstElem == -2 && ((fixAllM(i,0) >= from && fixAllM(i,0) <=  to ) ||(fixAllM(i,0) <= from && fixAllM(i,1) >= from ))){
                 // We found the first elem to delete
                 firstElem = i-1;
                 lastElem = i+ 1;
             }else if (firstElem != -2 && ((fixAllM(i,0) >= from && fixAllM(i,0) <=  to ) ||(fixAllM(i,0) <= from && fixAllM(i,1) >= from ))){
                // THere are still elements to delete
                 lastElem = i + 1;
             }else if(firstElem != -2 && ((fixAllM(i,0) >= from && fixAllM(i,1) >= from))){
                 break;
             }
        }

        if ((firstElem >= 0 && firstElem <= lastElem) || (firstElem == -1 && lastElem >= 1)){ // If the values are correct MERGE FIXATIONS
            startIndex = fixAllM( firstElem + 1,0);
            stopIndex =  fixAllM(lastElem-1,1);

            // Create row: newRow = [firstFix, lastFix, duration, averageX, averageY, variance, smoothPursuit, PupilDilation];
            double dur = ((roughM(stopIndex,0) - roughM(0,0))/ 1000 ) - ((roughM(startIndex,0) - roughM(0,0))/ 1000 );

            // Only use the points where eyes were detected:
            mat roughCutM;
            if (roughM.n_cols == 8){
                roughCutM= roughM.submat(from,2,to,7);
                GPMatrixFunctions::fncRemoveUndetectedValuesRough(&roughCutM);
            }else{
                 roughCutM= roughM.submat(from,2,to,5);
                 GPMatrixFunctions::fncRemoveUndetectedValuesRough(&roughCutM);
            }

            double averageX = mean((roughCutM.col(0) + roughCutM.col(2))/2);
            double averageY = mean((roughCutM.col(1) + roughCutM.col(3))/2);
            double variance = GPMatrixFunctions::fncCalculateRMSRough(&roughCutM, expWidth, expHeight, degPerPixel);
            double pupilDilation = 0;
            if (roughCutM.n_cols == 6){
                pupilDilation = (mean(roughCutM.col(4)) + mean(roughCutM.col(5)))/2;
            }

            mat row;
            row << startIndex << stopIndex << dur << averageX << averageY << variance << 0 << pupilDilation<< endr ;

            if (firstElem == -1 && lastElem == (int)fixAllM.n_rows){
                fixAllM = row;
            }else if(lastElem == (int)fixAllM.n_rows){ // the last element
                lastElem = lastElem -1;

                mat F1 = fixAllM( span(0, firstElem), span(0, fixAllM.n_cols-1) );
                fixAllM =  join_cols(F1,row);
            }else if (!(firstElem == -1 && lastElem >= 1)){

                mat F1 = fixAllM( span(0, firstElem), span(0, fixAllM.n_cols-1) );
                mat F2 = fixAllM(span(lastElem,fixAllM.n_rows-1),span(0, fixAllM.n_cols-1) );

               row = join_cols(F1,row);
               fixAllM = join_cols(row,F2);
             }else{ // Merge first fix

                fixAllM = fixAllM( span(lastElem, fixAllM.n_rows-1), span(0, fixAllM.n_cols-1) );
                fixAllM = join_cols(row,fixAllM);

            }



        }
    }

    return fixAllM;
}

// This method recalculates all the values for the fixations that are already detected.
// It is useful if we want to import fixations that were not created with GraFIX
mat GPFixationOperations::fncRecalculateFixations(mat roughM, mat fixAllM, int expWidth, int expHeight, double degPerPixel){
    for (uword i = 0; i < fixAllM.n_rows; ++i){

        // For each fixation, recalculate all the values
        // Create row: newRow = [firstFix, lastFix, duration, averageX, averageY, variance, smoothPursuit, PupilDilation];
        double dur = ((roughM(fixAllM(i,1),0) - roughM(0,0))/ 1000 ) - ((roughM(fixAllM(i,0),0) - roughM(0,0))/ 1000 );

        // Only use the points where eyes were detected:
        mat roughCutM;
        if (roughM.n_cols == 8){
            roughCutM = roughM.submat(fixAllM(i,0),2,fixAllM(i,1),7);
            GPMatrixFunctions::fncRemoveUndetectedValuesRough(&roughCutM);
        }else{
             roughCutM= roughM.submat(fixAllM(i,0),2,fixAllM(i,1),5);
             GPMatrixFunctions::fncRemoveUndetectedValuesRough(&roughCutM);
        }
        double averageX = mean((roughCutM.col(0) + roughCutM.col(2))/2);
        double averageY = mean((roughCutM.col(1) + roughCutM.col(3))/2);
        double variance = GPMatrixFunctions::fncCalculateRMSRough(&roughCutM, expWidth, expHeight, degPerPixel);
        double pupilDilation = 0;
        if (roughCutM.n_cols == 6){
            pupilDilation = (mean(roughCutM.col(4)) + mean(roughCutM.col(5)))/2;
        }

        fixAllM(i,2) = dur;
        fixAllM(i,3) = averageX;
        fixAllM(i,4) = averageY;
        fixAllM(i,5) = variance;
        fixAllM(i,7) = pupilDilation;
    }

    return fixAllM;
}


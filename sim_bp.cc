#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include "sim_bp.h"

using namespace std;

/*  argc holds the number of command line arguments
    argv[] holds the commands themselves

    Example:-
    sim bimodal 6 gcc_trace.txt
    argc = 4
    argv[0] = "sim"
    argv[1] = "bimodal"
    argv[2] = "6"
    ... and so on
*/


unsigned int Determine_Index_gshare(unsigned int address, int M, int N, int gbhr){

    address = address / (int)(pow(2,2));
    unsigned int index_part2 = address % (int)(pow(2, M-N));
    unsigned int N_bits = address / (int)(pow(2, M-N));
    unsigned int index_part1 = N_bits ^ gbhr;
    unsigned int index_ = index_part1 * (int)(pow(2, M-N)) + index_part2;
    unsigned int index_bits = index_ % (int)(pow(2, M));
    //cout<<" Index is "<<index_bits<<endl;
    return index_bits;

}

unsigned int Determine_Index_bimodal(unsigned int address, int m2){
    unsigned int mask_ = (int) (pow(2, m2)-1);
    unsigned int index_= (address >> 2) & mask_;

    return index_;
}

unsigned int Determine_Index_hybrid(unsigned int address, int k){
    address = address / (int)(pow (2,2));

    unsigned int index_ = address % (int)(pow(2, k));
    return index_;
}

void bimodal_predictor(unsigned int addr, int M2, char outcome, int *bimodal_PredictionTable, unsigned int &bimodal_prediction_count, unsigned int &bimodal_misprediction_count){
    unsigned int bimodal_index_ = Determine_Index_bimodal(addr, M2);
    bimodal_prediction_count++;
    int bimodal_branch_counter_ =bimodal_PredictionTable[bimodal_index_];

    int prediction_=0;
    if(bimodal_branch_counter_ >=2){
        prediction_ = 1;
    }
    else
    {
        prediction_ =0;
    }

    if(outcome == 't' && bimodal_PredictionTable[bimodal_index_] < 3){
        bimodal_PredictionTable[bimodal_index_]++;
    }

    if(outcome == 'n' && bimodal_PredictionTable[bimodal_index_] > 0){
        bimodal_PredictionTable[bimodal_index_]--;
    }

    if((prediction_ ==1 && outcome == 'n') || (prediction_ ==0 && outcome =='t')){
        bimodal_misprediction_count++;
    }
}

void gshare_predictor(unsigned int addr, int M, int N, char outcome, unsigned int &gbhr, int *gshare_PredictionTable, unsigned int &gshare_prediction_count, unsigned int &gshare_misprediction_count ){
        // Determine the branch's index into prediction table
        unsigned int gshare_index_= Determine_Index_gshare(addr, M, N, gbhr );
        gshare_prediction_count++;
        int prediction_ =0;
        int gshare_branch_counter_ = gshare_PredictionTable[gshare_index_];

        //Make a Prediction
        if(gshare_branch_counter_ >=2){
            prediction_ =1;
        }
        else
        {
            prediction_ =0;
        }

        //Update predictor  based on "actual" outcome
       if(outcome == 't' && gshare_PredictionTable[gshare_index_] < 3){
           gshare_PredictionTable[gshare_index_]++;

       }
       if(outcome == 'n' && gshare_PredictionTable[gshare_index_] > 0){
           gshare_PredictionTable[gshare_index_]--;
       }

       //Update global branch history register
       if(outcome == 't'){
            gbhr= gbhr/2 + (int)(pow(2, N-1));
       }
       if(outcome == 'n'){
           gbhr = gbhr/2;
       }
       //cout<<" Updated Global Branch History Register is "<<hex<<gbhr<<endl;
    
       if((prediction_ == 1 && outcome == 'n' )|| (prediction_ ==0 && outcome == 't')){
           gshare_misprediction_count++;
       }

}

void hybrid_branch_predictor(unsigned long int addr, int M1, int M2, int k, int N, unsigned int &gbhr, char outcome, unsigned int &hybrid_misprediction_count, unsigned int &hybrid_prediction_count, int *hybrid_ChooserTable, int *gshare_PredictionTable, int *bimodal_PredictionTable ){
    unsigned int hybrid_index_ = Determine_Index_hybrid(addr, k);
    
    hybrid_prediction_count ++;
    int hybrid_branch_counter_ = hybrid_ChooserTable[hybrid_index_];
    int hybrid_prediction_ =0;
    int gshare_prediction_ =0;
    int bimodal_prediction_=0;

    unsigned int gshare_index_ = Determine_Index_gshare(addr, M1, N, gbhr);    
    int gshare_branch_counter_ = gshare_PredictionTable[gshare_index_];
    //Make a Prediction
    if(gshare_branch_counter_ >=2){
        gshare_prediction_ =1;
    }
    else{
        gshare_prediction_ =0;
    }

    unsigned int bimodal_index_ = Determine_Index_bimodal(addr, M2);
    int bimodal_branch_counter_ =bimodal_PredictionTable[bimodal_index_];
    // Make a Prediction
    if(bimodal_branch_counter_ >=2){
        bimodal_prediction_ = 1;
    }
    else{
        bimodal_prediction_ =0;
    }

    //Update predictor  based on "actual" outcome
    if( hybrid_branch_counter_ >= 2){
        if((outcome == 't')  && (gshare_PredictionTable[gshare_index_] < 3)){
            gshare_PredictionTable[gshare_index_]++;
        }
        if((outcome == 'n') && (gshare_PredictionTable[gshare_index_] > 0)){
            gshare_PredictionTable[gshare_index_]--;
        }

        hybrid_prediction_ = gshare_prediction_;
    }

    else{
        if((outcome == 't') && (bimodal_PredictionTable[bimodal_index_] < 3)){
            bimodal_PredictionTable[bimodal_index_]++;
        }

        if((outcome == 'n') &&  (bimodal_PredictionTable[bimodal_index_] > 0)){
            bimodal_PredictionTable[bimodal_index_]--;
        }
        hybrid_prediction_ = bimodal_prediction_;
    }

    //Update global branch history register irrespective of bimodal or gshare
    if(outcome == 't'){
        gbhr= gbhr/2 + (int)(pow(2, N-1));
    }
    if(outcome == 'n'){
        gbhr = gbhr/2;
    }

        
    //Update Branch Chooser counter based on actual outcome
    if(outcome == 't'){
        if(bimodal_prediction_ == 0 && gshare_prediction_ ==1 && hybrid_ChooserTable[hybrid_index_] < 3 ){
            hybrid_ChooserTable[hybrid_index_]++;
        }
        
        if(bimodal_prediction_ ==1 && gshare_prediction_ ==0 && hybrid_ChooserTable[hybrid_index_] >0){
            hybrid_ChooserTable[hybrid_index_]--;
        }
    }
    
    if(outcome == 'n'){
        if(bimodal_prediction_ ==0 && gshare_prediction_ ==1 && hybrid_ChooserTable[hybrid_index_] > 0){
            hybrid_ChooserTable[hybrid_index_]--;
       }
       
       if(bimodal_prediction_ ==1 && gshare_prediction_ ==0 && hybrid_ChooserTable[hybrid_index_] <3){
           hybrid_ChooserTable[hybrid_index_]++;
       }
    }

    if((hybrid_prediction_ ==1 && outcome =='n') || (hybrid_prediction_ ==0 && outcome =='t')){
        hybrid_misprediction_count++;
    }
}

int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    bp_params params;       // look at sim_bp.h header file for the the definition of struct bp_params
    char outcome;           // Variable holds branch outcome
    unsigned long int addr; // Variable holds the address read from input file

    unsigned int gshare_gbhr=0;
    
    if (!(argc == 4 || argc == 5 || argc == 7))
    {
        printf("Error: Wrong number of inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }
    
    params.bp_name  = argv[1];
    
    // strtoul() converts char* to unsigned long. It is included in <stdlib.h>
    if(strcmp(params.bp_name, "bimodal") == 0)              // Bimodal
    {
        if(argc != 4)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M2       = strtoul(argv[2], NULL, 10);
        trace_file      = argv[3];
        //printf("COMMAND\n%s %s %lu %s\n", argv[0], params.bp_name, params.M2, trace_file);
    }
    else if(strcmp(params.bp_name, "gshare") == 0)          // Gshare
    {
        if(argc != 5)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M1       = strtoul(argv[2], NULL, 10);
        params.N        = strtoul(argv[3], NULL, 10);
        trace_file      = argv[4];
        //printf("COMMAND\n%s %s %lu %lu %s\n", argv[0], params.bp_name, params.M1, params.N, trace_file);

    }
    else if(strcmp(params.bp_name, "hybrid") == 0)          // Hybrid
    {
        if(argc != 7)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.K        = strtoul(argv[2], NULL, 10);
        params.M1       = strtoul(argv[3], NULL, 10);
        params.N        = strtoul(argv[4], NULL, 10);
        params.M2       = strtoul(argv[5], NULL, 10);
        trace_file      = argv[6];
        //printf("COMMAND\n%s %s %lu %lu %lu %lu %s\n", argv[0], params.bp_name, params.K, params.M1, params.N, params.M2, trace_file);

    }
    else
    {
        printf("Error: Wrong branch predictor name:%s\n", params.bp_name);
        exit(EXIT_FAILURE);
    }


    //For g-share
    unsigned int gshare_prediction_count = 0;
    unsigned int gshare_misprediction_count =0;
    unsigned int M1 = params.M1;
    unsigned int N = params.N;
    unsigned int gshare_table_size= (int) (pow(2,M1));
    //cout<<"Table size is"<<table_size<<endl;
    int gshare_PredictionTable[gshare_table_size];
    for(unsigned int i=0; i < gshare_table_size ; i++){
        gshare_PredictionTable[i]=1;
         //cout <<i<<"\t"<<PredictionTable[i]<<endl;
     }

    //For Bi-Modal
    unsigned int bimodal_prediction_count=0;
    unsigned int bimodal_misprediction_count =0;
    unsigned int bimodal_exponent = params.M2;
    unsigned int bimodal_table_size = pow(2, bimodal_exponent);
    int bimodal_PredictionTable[bimodal_table_size];
    for(unsigned int i=0; i< bimodal_table_size; i++){
        bimodal_PredictionTable[i]=2;
    }


    //For Hybrid
    unsigned int hybrid_prediction_count=0;
    unsigned int hybrid_misprediction_count =0;
    unsigned int hybrid_exponent = params.K;
    unsigned int M2= params.M2;
    //unsigned int M1= params.M1;
    //unsigned int N= params.N;
    unsigned int hybrid_table_size = pow(2, hybrid_exponent);
    int hybrid_ChooserTable[hybrid_table_size];
    for(unsigned int i=0; i< hybrid_table_size; i++){
        hybrid_ChooserTable[i]=1;
    }





    //intialize the prediction table to 2
    //unsigned long int misprediction_count =0;
    // for(unsigned int i=0; i < gshare_table_size ; i++){
    //      gshare_PredictionTable[i]=2;
    //      //cout <<i<<"\t"<<PredictionTable[i]<<endl;
    //  }
    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }
    
    char str[2];
    while(fscanf(FP, "%lx %s", &addr, str) != EOF)
    {
        
        outcome = str[0];
        if (outcome == 't');
            //printf("%lx %s\n", addr, "t");           // Print and test if file is read correctly
        else if (outcome == 'n');
            //printf("%lx %s\n", addr, "n");          // Print and test if file is read correctly
        /*************************************
            Add branch predictor code here
        **************************************/
       if(strcmp(params.bp_name, "hybrid") == 0){
           hybrid_branch_predictor(addr, M1, M2, hybrid_exponent, N, gshare_gbhr, outcome, hybrid_misprediction_count, hybrid_prediction_count, hybrid_ChooserTable, gshare_PredictionTable, bimodal_PredictionTable);
       }
       if(strcmp(params.bp_name, "gshare") == 0){
           gshare_predictor(addr, M1, N, outcome, gshare_gbhr, gshare_PredictionTable, gshare_prediction_count, gshare_misprediction_count);
       }
       if(strcmp(params.bp_name, "bimodal") == 0){
           bimodal_predictor(addr, M2, outcome, bimodal_PredictionTable, bimodal_prediction_count,bimodal_misprediction_count);
       }

    }

    //print 
    if(strcmp(params.bp_name, "hybrid") == 0) {
        double hybrid_miss_rate=0.0;
        hybrid_miss_rate = (double)(hybrid_misprediction_count) / (double) (hybrid_prediction_count);
        //cout<<"OUTPUT"<<endl;
        //cout<<" number of predictions:"<<"\t"<<hybrid_prediction_count<<endl;
        //cout<<" number of mispredictions: "<<"\t"<<hybrid_misprediction_count<<endl;
        //cout<<setprecision(4)<<" misprediction rate: "<<"\t"<<hybrid_miss_rate *100 <<"%"<<endl;
        printf("%0.2f\n",hybrid_miss_rate*100);
        // cout<<"FINAL CHOOSER CONTENTS"<<endl;
        // for(unsigned int i=0; i < hybrid_table_size; i++){
        //     cout<<i<<"\t"<<hybrid_ChooserTable[i]<<endl;  
        // }
        // cout<<"FINAL GSHARE CONTENTS"<<endl;
        // for(unsigned int i=0; i < gshare_table_size; i++){
        //     cout<<i<<"\t"<<gshare_PredictionTable[i]<<endl;  
        // }
        // cout<<"FINAL BIMODAL CONTENTS"<<endl;
        // for(unsigned int i=0; i < bimodal_table_size; i++){
        //     cout<<i<<"\t"<<bimodal_PredictionTable[i]<<endl;  
        // }

    }

    if(strcmp(params.bp_name, "gshare") == 0) {
        double gshare_miss_rate=0.0;
        gshare_miss_rate = (double)(gshare_misprediction_count) / (double) (gshare_prediction_count);
        //cout<<"OUTPUT"<<endl;
        //cout<<" number of predictions:"<<"\t"<<gshare_prediction_count<<endl;
        //cout<<" number of mispredictions: "<<"\t"<<gshare_misprediction_count<<endl;
        //cout<<setprecision(4)<<" misprediction rate: "<<"\t"<<hybrid_miss_rate *100 <<"%"<<endl;
        printf("%0.2f\n",gshare_miss_rate*100);
        // cout<<"FINAL GSHARE CONTENTS"<<endl;
        // for(unsigned int i=0; i < gshare_table_size; i++){
        //     cout<<i<<"\t"<<gshare_PredictionTable[i]<<endl;  
        // }
    }
    if(strcmp(params.bp_name, "bimodal") == 0){
        double bimodal_miss_rate=0.0;
        bimodal_miss_rate = (double)(bimodal_misprediction_count) / (double) (bimodal_prediction_count);
        //cout<<"OUTPUT"<<endl;
        //cout<<" number of predictions:"<<"\t"<<bimodal_prediction_count<<endl;
        //cout<<" number of mispredictions: "<<"\t"<<bimodal_misprediction_count<<endl;
        //cout<<setprecision(4)<<" misprediction rate: "<<"\t"<<hybrid_miss_rate *100 <<"%"<<endl;
        printf("%0.2f\n",bimodal_miss_rate*100);
        // cout<<"FINAL BIMODAL CONTENTS"<<endl;
        // for(unsigned int i=0; i < bimodal_table_size; i++){
        //     cout<<i<<"\t"<<bimodal_PredictionTable[i]<<endl;
        // }
    }

    return 0;
}

//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include <math.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "Pranav Gangwar";
const char *studentID   = "A59011245";
const char *email       = "pgangwar@ucsd.edu";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//

uint32_t ghistory;
uint32_t lhistory;
uint8_t gpred_table[8192]; //2^13 -> max size
uint8_t pattern_history_table[2048]; //2^13 -> max size
uint8_t branch_history_table[2048]; //2^13 -> max size
uint8_t meta_predictor_table[2048]; //2^13 -> max size

uint8_t twoBitCounterBits = 9;
uint8_t two_bit_hist[4096]; //2^12 -> max size
uint8_t hierarchical_select_table[8192]; //2^13 -> max size

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

uint8_t gshare_predict(uint32_t pc)
{
    uint8_t pred, result;
    ghistory = ghistory << (32-ghistoryBits); 
    ghistory = ghistory >> (32-ghistoryBits);
    pc = pc >> 2;
    pc = pc << (32-ghistoryBits);
    pc = pc >> (32-ghistoryBits);

    pred = gpred_table[pc ^ ghistory];

    switch (pred) {
    case SN:
        result = NOTTAKEN;
        break;
    case WN:
        result = NOTTAKEN; 
        break;
    case WT:
        result = TAKEN; 
        break;
    case ST:
        result = TAKEN; 
        break;
    default:
        break;
  }

    return result;
    
}


uint8_t pshare_predict(uint32_t pc)
{
    uint8_t pred, result;
    pc = pc >> 2;
    pc = pc << (32-pcIndexBits);
    pc = pc >> (32-pcIndexBits);

    lhistory = pattern_history_table[pc];
    lhistory = lhistory << (32-lhistoryBits); 
    lhistory = lhistory >> (32-lhistoryBits);
    pred = branch_history_table[pc ^ lhistory];

    switch (pred) {
    case SN:
        result = NOTTAKEN;
        break;
    case WN:
        result = NOTTAKEN; 
        break;
    case WT:
        result = TAKEN; 
        break;
    case ST:
        result = TAKEN; 
        break;
    default:
        break;
  }

    return result;
    
}


uint8_t tournament_predict(uint32_t pc)
{
    uint8_t gshare_prediction, pshare_prediction, select;
    
    gshare_prediction = gshare_predict(pc);
    pshare_prediction = pshare_predict(pc);
    
    pc = pc >> 2;
    pc = pc << (32-pcIndexBits);
    pc = pc >> (32-pcIndexBits);

    select=meta_predictor_table[pc];

    switch (select) {
    case 0:
        return gshare_prediction;
    case 1:
        return gshare_prediction;
    case 2:
        return pshare_prediction;
    case 3:
        return pshare_prediction;
    default:
        break;
  }


}

uint8_t two_bit_predict(uint32_t pc)
{
    uint8_t result;

    pc = pc >> 2;
    pc = pc << (32-twoBitCounterBits);
    pc = pc >> (32-twoBitCounterBits);

    switch (two_bit_hist[pc]) {
    case SN:
        result = NOTTAKEN;
        break;
    case WN:
        result = NOTTAKEN; 
        break;
    case WT:
        result = TAKEN; 
        break;
    case ST:
        result = TAKEN; 
        break;
    default:
        break;
  }

    return result;
    
}


uint8_t custom_predict(uint32_t pc)
{
    uint8_t gshare_prediction, pshare_prediction, two_bit_prediction, select;
    gshare_prediction = gshare_predict(pc);
    pshare_prediction = pshare_predict(pc);
    two_bit_prediction = two_bit_predict(pc);

    pc = pc >> 2;
    pc = pc << (32-ghistoryBits);
    pc = pc >> (32-ghistoryBits);

    select=hierarchical_select_table[pc];

    switch (select) {
    case 0:
        return two_bit_prediction;
    case 1:
        return two_bit_prediction;
    case 2:
        return two_bit_prediction;
    case 3:
        return pshare_prediction;
    case 4:
        return pshare_prediction;
    case 5:
        return pshare_prediction;
    case 6:
        return pshare_prediction;
    case 7:
        return gshare_prediction;
    default:
        break;
  }

}


// Initialize the predictor
//
void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
    int i;
    ghistory = 0;
    for (i=0; i<8192; i++)
    {
        gpred_table[i] = 1;
    }

    for (i=0; i<2048; i++)
    {
        pattern_history_table[i] = 0;
    }

    for (i=0; i<2048; i++)
    {
        branch_history_table[i] = 1;
    }

    for (i=0; i<2048; i++)
    {
        meta_predictor_table[i] = 1;
    }

    for (i=0; i<4096; i++)
    {
        two_bit_hist[i] = 1;
    }

    for (i=0; i<8192; i++)
    {
        hierarchical_select_table[i] = 0;
    }

}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
        return TAKEN;
    case GSHARE:
        return gshare_predict(pc); 
    case TOURNAMENT:
        return tournament_predict(pc); 
    case CUSTOM:
        return custom_predict(pc); 
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//


void gshare_train(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //

    uint8_t pred;

    //ghistory = ghistory << (32-ghistoryBits); 
    //ghistory = ghistory >> (32-ghistoryBits);
    pc = pc >> 2;
    pc = pc << (32-ghistoryBits);
    pc = pc >> (32-ghistoryBits);
 
    pred = gpred_table[pc ^ ghistory];

    switch (pred) {
    case SN:
            if (outcome == TAKEN) 
                gpred_table[pc ^ ghistory] = WN;
            break;
    case WN:
            if (outcome == TAKEN) 
                gpred_table[pc ^ ghistory] = WT;
            else
                gpred_table[pc ^ ghistory] = SN;
            break;
    case WT:
            if (outcome == TAKEN) 
                gpred_table[pc ^ ghistory] = ST;
            else
                gpred_table[pc ^ ghistory] = WN;
            break;
    case ST:
            if (outcome == NOTTAKEN) 
                gpred_table[pc ^ ghistory] = WT;
            break;
    default:
            break;

}

    ghistory = (ghistory << 1) | outcome;

}


void pshare_train(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //

    uint8_t pred;

    //ghistory = ghistory << (32-ghistoryBits); 
    //ghistory = ghistory >> (32-ghistoryBits);
    pc = pc >> 2;
    pc = pc << (32-pcIndexBits);
    pc = pc >> (32-pcIndexBits);
 

    //lhistory = pattern_history_table[pc];
    //lhistory = lhistory << (32-lhistoryBits); 
    //lhistory = lhistory >> (32-lhistoryBits);
    pred = branch_history_table[pc ^ lhistory];

    switch (pred) {
    case SN:
            if (outcome == TAKEN) 
                branch_history_table[pc ^ lhistory] = WN;
            break;
    case WN:
            if (outcome == TAKEN) 
                branch_history_table[pc ^ lhistory] = WT;
            else
                branch_history_table[pc ^ lhistory] = SN;
            break;
    case WT:
            if (outcome == TAKEN) 
                branch_history_table[pc ^ lhistory] = ST;
            else
                branch_history_table[pc ^ lhistory] = WN;
            break;
    case ST:
            if (outcome == NOTTAKEN) 
                branch_history_table[pc ^ lhistory] = WT;
            break;
    default:
            break;

}
    pattern_history_table[pc] = (lhistory << 1) | outcome;

}


void tournament_train(uint32_t pc, uint8_t outcome)
{
    uint8_t gshare_pred, pshare_pred;
    uint32_t pc_gshare, pc_pshare;
    pc_gshare = pc;
    pc_gshare = pc_gshare >> 2;
    pc_gshare = pc_gshare << (32-ghistoryBits);
    pc_gshare = pc_gshare >> (32-ghistoryBits);
    gshare_pred = gpred_table[pc_gshare ^ ghistory];

    pc_pshare = pc;
    pc_pshare = pc_pshare >> 2;
    pc_pshare = pc_pshare << (32-pcIndexBits);
    pc_pshare = pc_pshare >> (32-pcIndexBits);
    pshare_pred = branch_history_table[pc_pshare ^ lhistory];

    switch(meta_predictor_table[pc_pshare]) {
    case 0:
            if (((outcome == TAKEN) && (gshare_pred == SN || gshare_pred == WN) && (pshare_pred == ST || pshare_pred == WT)) || ((outcome == NOTTAKEN) && (gshare_pred == ST || gshare_pred == WT) && (pshare_pred == SN || pshare_pred == WN)))
                meta_predictor_table[pc_pshare] = 1;
            break;
    case 1:
            if (((outcome == TAKEN) && (gshare_pred == SN || gshare_pred == WN) && (pshare_pred == ST || pshare_pred == WT)) || ((outcome == NOTTAKEN) && (gshare_pred == ST || gshare_pred == WT) && (pshare_pred == SN || pshare_pred == WN)))
                meta_predictor_table[pc_pshare] = 2;
            else if (((outcome == NOTTAKEN) && (gshare_pred == SN || gshare_pred == WN) && (pshare_pred == ST || pshare_pred == WT)) || ((outcome == TAKEN) && (gshare_pred == ST || gshare_pred == WT) && (pshare_pred == SN || pshare_pred == WN)))
                meta_predictor_table[pc_pshare] = 0;
            break;
    case 2:
            if (((outcome == TAKEN) && (gshare_pred == SN || gshare_pred == WN) && (pshare_pred == ST || pshare_pred == WT)) || ((outcome == NOTTAKEN) && (gshare_pred == ST || gshare_pred == WT) && (pshare_pred == SN || pshare_pred == WN)))
                meta_predictor_table[pc_pshare] = 3;
            else if (((outcome == NOTTAKEN) && (gshare_pred == SN || gshare_pred == WN) && (pshare_pred == ST || pshare_pred == WT)) || ((outcome == TAKEN) && (gshare_pred == ST || gshare_pred == WT) && (pshare_pred == SN || pshare_pred == WN)))
                meta_predictor_table[pc_pshare] = 1;
            break;
    case 3:
            if (((outcome == NOTTAKEN) && (gshare_pred == SN || gshare_pred == WN) && (pshare_pred == ST || pshare_pred == WT)) || ((outcome == TAKEN) && (gshare_pred == ST || gshare_pred == WT) && (pshare_pred == SN || pshare_pred == WN)))
                meta_predictor_table[pc_pshare] = 2;
            break;
    default:
            break;

    }

    gshare_train(pc, outcome);
    pshare_train(pc, outcome);

}


void two_bit_train(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //

    //ghistory = ghistory << (32-ghistoryBits); 
    //ghistory = ghistory >> (32-ghistoryBits);
    pc = pc >> 2;
    pc = pc << (32-twoBitCounterBits);
    pc = pc >> (32-twoBitCounterBits);

    switch (two_bit_hist[pc]) {
    case SN:
            if (outcome == TAKEN) 
                two_bit_hist[pc] = WN;
            break;
    case WN:
            if (outcome == TAKEN) 
                two_bit_hist[pc] = WT;
            else
                two_bit_hist[pc] = SN;
            break;
    case WT:
            if (outcome == TAKEN) 
                two_bit_hist[pc] = ST;
            else
                two_bit_hist[pc] = WN;
            break;
    case ST:
            if (outcome == NOTTAKEN) 
                two_bit_hist[pc] = WT;
            break;
    default:
            break;

}

}

void custom_train(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //


    uint8_t gshare_pred, pshare_pred, pc_two_bit_counter_pred;
    uint32_t pc_gshare, pc_pshare, pc_two_bit_counter;
   
    pc_gshare = pc;
    pc_gshare = pc_gshare >> 2;
    pc_gshare = pc_gshare << (32-ghistoryBits);
    pc_gshare = pc_gshare >> (32-ghistoryBits);
    gshare_pred = gpred_table[pc_gshare ^ ghistory];
   
    pc_pshare = pc;
    pc_pshare = pc_pshare >> 2;
    pc_pshare = pc_pshare << (32-pcIndexBits);
    pc_pshare = pc_pshare >> (32-pcIndexBits);
    pshare_pred = branch_history_table[pc_pshare ^ lhistory];

    pc_two_bit_counter = pc;
    pc_two_bit_counter = pc_two_bit_counter >> 2;
    pc_two_bit_counter = pc_two_bit_counter << (32-twoBitCounterBits);
    pc_two_bit_counter = pc_two_bit_counter >> (32-twoBitCounterBits);
    pc_two_bit_counter_pred = two_bit_hist[pc_two_bit_counter]; 



    switch(hierarchical_select_table[pc_gshare]) {
    case 0:
            if (((outcome == TAKEN) && (pc_two_bit_counter_pred == SN || pc_two_bit_counter_pred == WN)) || ((outcome == NOTTAKEN) && (pc_two_bit_counter_pred == ST || pc_two_bit_counter_pred == WT)))
                hierarchical_select_table[pc_gshare] = 1;
            two_bit_train(pc, outcome);
            break;
    case 1:
            if (((outcome == TAKEN) && (pc_two_bit_counter_pred == SN || pc_two_bit_counter_pred == WN)) || ((outcome == NOTTAKEN) && (pc_two_bit_counter_pred == ST || pc_two_bit_counter_pred == WT)))
                hierarchical_select_table[pc_gshare] = 2;
            //else if (((outcome == TAKEN) && (pc_two_bit_counter_pred == ST || pc_two_bit_counter_pred == WT)) || ((outcome == NOTTAKEN) && (pc_two_bit_counter_pred == SN || pc_two_bit_counter_pred == WN)))
            //    hierarchical_select_table[pc_gshare] = 0;
            two_bit_train(pc, outcome);
            break;
    case 2:
            if (((outcome == TAKEN) && (pc_two_bit_counter_pred == SN || pc_two_bit_counter_pred == WN)) || ((outcome == NOTTAKEN) && (pc_two_bit_counter_pred == ST || pc_two_bit_counter_pred == WT)))
                hierarchical_select_table[pc_gshare] = 3;
            //else if (((outcome == TAKEN) && (pc_two_bit_counter_pred == ST || pc_two_bit_counter_pred == WT)) || ((outcome == NOTTAKEN) && (pc_two_bit_counter_pred == SN || pc_two_bit_counter_pred == WN)))
            //    hierarchical_select_table[pc_gshare] = 1;
            two_bit_train(pc, outcome);
            break;
    case 3:
            if (((outcome == TAKEN) &&  (pshare_pred == SN || pshare_pred == WN)) || ((outcome == NOTTAKEN) && (pshare_pred == ST || pshare_pred == WT)))
                hierarchical_select_table[pc_gshare] = 4;
            pshare_train(pc, outcome);
            break;
    case 4:
            if (((outcome == TAKEN) &&  (pshare_pred == SN || pshare_pred == WN)) || ((outcome == NOTTAKEN) && (pshare_pred == ST || pshare_pred == WT)))
                hierarchical_select_table[pc_gshare] = 5;
            else if (((outcome == TAKEN) &&  (pshare_pred == ST || pshare_pred == WT)) || ((outcome == NOTTAKEN) && (pshare_pred == SN || pshare_pred == WN)))
                hierarchical_select_table[pc_gshare] = 3;
            pshare_train(pc, outcome);
            break;
    case 5:
            if (((outcome == TAKEN) &&  (pshare_pred == SN || pshare_pred == WN)) || ((outcome == NOTTAKEN) && (pshare_pred == ST || pshare_pred == WT)))
                hierarchical_select_table[pc_gshare] = 6;
            else if (((outcome == TAKEN) &&  (pshare_pred == ST || pshare_pred == WT)) || ((outcome == NOTTAKEN) && (pshare_pred == SN || pshare_pred == WN)))
                hierarchical_select_table[pc_gshare] = 4;
            pshare_train(pc, outcome);
            break;
    case 6:
            if (((outcome == TAKEN) &&  (pshare_pred == SN || pshare_pred == WN)) || ((outcome == NOTTAKEN) && (pshare_pred == ST || pshare_pred == WT)))
                hierarchical_select_table[pc_gshare] = 7;
            else if (((outcome == TAKEN) &&  (pshare_pred == ST || pshare_pred == WT)) || ((outcome == NOTTAKEN) && (pshare_pred == SN || pshare_pred == WN)))
                hierarchical_select_table[pc_gshare] = 5;
            pshare_train(pc, outcome);
            break;

    case 7:
            gshare_train(pc, outcome);
            break;
    default:
            break;

    }

}



void
train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //

  switch (bpType) {
    case STATIC:
        break;
    case GSHARE:
        gshare_train(pc, outcome); 
        break;
    case TOURNAMENT:
        tournament_train(pc, outcome);
        break;
    case CUSTOM:
        custom_train(pc, outcome);
        break;
    default:
      break;
}
    

}


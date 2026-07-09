#ifndef LOCATIONREFERENCE_H
#define LOCATIONREFERENCE_H

/*
    This is a file that stores large arrays of information needed
    to parse location, without needing hundreds of lines before the code 
    starts in the location parsing module.
*/


#include <unistd.h>
#include <stdlib.h>
#include <string.h>

// number of states stored
#define PARLOCNUMSTATES 64
// number of multi-state cue words
#define PARLOCNUMMULTISTATES 11
// number of misspellings
#define PARLOCALTLEN 32


/* An array of US States and Canadian Provinces
   Each state abbreviation is next to its full name
   For normalization purposes, when storing state, we should 
   store this information as the two letter code.

   From the array, we just subtract one from odd indices
*/
const char* states [] = {   "AL", "ALABAMA", 
                            "AK", "ALASKA", 
                            "AZ", "ARIZONA", 
                            "AR", "ARKANSAS", 
                            "CA", "CALIFORNIA", 
                            "CO", "COLORADO", 
                            "CT", "CONNECTICUT", 
                            "DE", "DELAWARE", 
                            "DC", "DISTRICT OF COLUMBIA",
                            "FL", "FLORIDA", 
                            "GA", "GEORGIA", 
                            "HI", "HAWAII",
                            "ID", "IDAHO", 
                            "IL", "ILLINOIS", 
                            "IN", "INDIANA", 
                            "IA", "IOWA", 
                            "KS", "KANSAS",
                            "KY", "KENTUCKY", 
                            "LA", "LOUISIANA", 
                            "ME", "MAINE", 
                            "MD", "MARYLAND", 
                            "MA", "MASSACHUSETTS", 
                            "MI", "MICHIGAN", 
                            "MN", "MINNESOTA", 
                            "MS", "MISSISSIPPI", 
                            "MO", "MISSOURI",
                            "MT", "MONTANA",
                            "NE", "NEBRASKA",
                            "NV", "NEVADA",
                            "NH", "NEW HAMPSHIRE", 
                            "NJ", "NEW JERSEY",
                            "NM", "NEW MEXICO",
                            "NY", "NEW YORK",
                            "NC", "NORTH CAROLINA",
                            "ND", "NORTH DAKOTA",
                            "OH", "OHIO",
                            "OK", "OKLAHOMA",
                            "OR", "OREGON",
                            "PA", "PENNSYLVANIA", 
                            "RI", "RHODE ISLAND",
                            "SC", "SOUTH CAROLINA",
                            "SD", "SOUTH DAKOTA",
                            "TN", "TENNESSEE", 
                            "TX", "TEXAS",
                            "UT", "UTAH",
                            "VT", "VERMONT",
                            "VA", "VIRGINIA", 
                            "WA", "WASHINGTON",
                            "WV", "WEST VIRGINIA", 
                            "WI", "WISCONSIN", 
                            "WY", "WYOMING",
                            "AB", "ALBERTA",
                            "BC", "BRITISH COLUMBIA",
                            "MB", "MANITOBA",
                            "NB", "NEW BRUNSWICK",
                            "NL", "NEWFOUNDLAND AND LABRADOR",
                            "NS", "NOVA SCOTIA",
                            "NT", "NORTHWEST TERRITORIES",
                            "NU", "NUNAVUT", // NEW
                            "ON", "ONTARIO",
                            "PE", "PRINCE EDWARD ISLAND", 
                            "QC", "QUEBEC",
                            "SK", "SASKATCHEWAN", 
                            "YT", "YUKON"
                        };


// for state/province/territory names that are not
const char* multistateCue [] = { "DISTRICT", "NEW", "NORTH", "RHODE", "SOUTH", "WEST", "BRITISH", "NEWFOUNDLAND",
                                    "NOVA", "NORTHWEST", "PRINCE"};

// the number of words in the above strings' corresponding names
// we are greatly benefitted by that any prefixes that have more than one corresponding state
// all share the same number of words. This implementation must be changed then if states is updated
const int multistateCueLength [] = { 3, 2, 2, 2, 2, 2, 2, 3, 2, 2, 3 };

// An array of common state and province misspellings or alternate names
const char* alternateStates [] = {  "CONNETICUT", 
                                    "DISTRICT OF COLOMBIA",
                                    "HAWAI'I", "HAWIIA", 
                                    "LOUISIANNA",
                                    "MASSACHUSSETTS", "MASSECHUSSETS", "MASSECHUSETTS", "MASSECHUSSETTS", 
                                    "MISSISIPPI", "MISISSIPPI", "MISSISSIPI", "MISISIPPI", "MISISSIPI", 
                                        "MISSISIPI", "MISISIPI",
                                    "NEW HAMSHIRE",
                                    "PENSYLVANIA", 
                                    "TENESSEE", "TENNESEE",
                                    "VIRGINNIA",
                                    "WEST VIRGINNIA",
                                    "WISCONSON",
                                    "PRINCE EDWARD",
                                    "SASKETCHEWAN", "SASKATCHAWAN", "SASKETCHAWAN",
                                    "NEWFOUNDLAND", "LABRADOR",
                                    "NONAVUT", "NUNAVIT", "NONAVIT"
                                };

// This array stores the canonical names of the misspellings above
// So for instance 
/* 
    alternateStates[2] = "HAWIIA"
    which maps to
    alternateStatesCanonical[2] = "HI"
*/
const char* alternateStatesCanonical[] =
                                {   "CT",
                                    "DC",
                                    "HI", "HI",
                                    "LA",
                                    "MA", "MA", "MA", "MA",
                                    "MS", "MS", "MS", "MS", "MS", "MS", "MS", 
                                    "NH",
                                    "PA",
                                    "TN", "TN",
                                    "VA",
                                    "WV",
                                    "WI",
                                    "PE",
                                    "SK", "SK", "SK", 
                                    "NL", "NL",
                                    "NU", "NU", "NU"
                                };

#endif
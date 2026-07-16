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
// number of street suffixes
#define PARLOCSTRSUFS 569
// number of street directionals (N, NW, SW, ETC)
#define PARLOCSTRDIRS 24
// number of valid country spellings
#define PARLOCCOUNTRIES 5


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


const char * countries [] = {   "US",
                                "USA",
                                "United States",
                                "United States of America",
                                "Canada"
};

// The following information is taken from https://github.com/0syntrax0/go-address-parser/blob/master/street.go,
// which is licensed under the following MIT License:

/*
    MIT License

    Copyright (c) 2020 Carlos A Saavedra

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

const char* streetDirectionals [] = {   "N", "NORTH", "NORTE",
                                        "E", "EAST", "ESTE",
                                        "W", "WEST", "OESTE",
                                        "S", "SOUTH", "SUR",
                                        "NE", "NORTHEAST", "NORESTE",
                                        "NW", "NORTHWEST", "NOROESTE",
                                        "SE", "SOUTHEAST", "SURESTE",
                                        "SW", "SOUTHWEST", "SUROESTE"
                                    };

                                // the list provided sorted the list in alphabetical order,
                                // but I put the most common suffixes at the start for faster
                                // positive returns. Unfortunately, negative matches will still
                                // take a little while.
const char* streetSuffixes[] = {"ROAD", "RD", "STREET", "ST", "WAY", "WY", "AVENUE", "AVE",
                                "DRIVE", "DR", "LANE", "LN", "PLACE", "PL", "TERRACE", "TERR", "TER",
                                "PATH", "BOULEVARD", "BLVD", "COURT", "CT", "CRESCENT", "CRES",
                                "CIRCLE", "CIR", "HIGHWAY", "HWY", "PARKWAY", "PKWY", "AVENIDA", 
                                "CALLE", "CLL", "PASEO", "PSO", "VEREDA", "VER", "CERRADA", "CER", 
                                "CIRCULO", "CAMINO", "CAM",
    
                                "ALLEY", "ALY", "ALLY", "ANEX", "ANX", "ANNEX", "ANNX",
                                "ARCADE", "ARC", "AVEN", "AVENU", "AVN",
                                "AVNUE", "BAYOU", "BYU", "BEACH", "BCH", "BEND", "BND",
                                "BLUFF", "BLF", "BLUF", "BLUFFS", "BLFS", "BOTTOM", "BTM",
                                "BOT", "BOTTM", "BOUL", "BOULV", "BRANCH",
                                "BR", "BRNCH", "BRIDGE", "BRG", "BRDGE", "BROOK", "BRK", "BROOKS",
                                "BRKS", "BURG", "BG", "BURGS", "BGS", "BYPASS", "BYP", "BYPA",
                                "BYPAS", "BYPS", "CAMP", "CP", "CMP", "CANYON", "CYN", "CNYN",
                                "CAPE", "CPE", "CAUSEWAY", "CSWY", "CAUSWA", "CENTER", "CTR",
                                "CENT", "CENTR", "CENTRE", "CNTER", "CNTR", "CENTERS", "CTRS",
                                "CIRC", "CIRCL", "CRCL", "CRCLE", "CIRCLES",
                                "CIRS", "CLIFF", "CLF", "CLIFFS", "CLFS", "CLUB", "CLB", "COMMON",
                                "CMN", "COMMONS", "CMNS", "CORNER", "COR", "CORNERS", "CORS", "COURSE",
                                "CRSE", "COURTS", "CTS", "COVE", "CV", "COVES", "CVS",
                                "CREEK", "CRK",  "CRSENT", "CRSNT", "CREST", "CRST",
                                "CROSSING", "XING", "CRSSNG", "CROSSROAD", "XRD", "CROSSROADS", "XRDS",
                                "CURVE", "CURV", "DALE", "DL", "DAM", "DM", "DIVIDE", "DV", "DVD", "DIV",
                                "DRIV", "DRV", "DRIVES", "DRS", "ESTATE", "EST", "ESTATES",
                                "ESTS", "EXPRESSWAY", "EXPY", "EXPR", "EXPRESS", "EXPW", "EXTENSION", "EXT",
                                "EXTN", "EXTNSN", "EXTENSIONS", "EXTS", "FALL", "FALLS", "FLS", "FERRY",
                                "FRY", "FRRY", "FIELD", "FLD", "FIELDS", "FLDS", "FLAT", "FLT", "FLATS", "FLTS",
                                "FORD", "FRD", "FORDS", "FRDS", "FOREST", "FRST", "FORESTS", "FORGE",
                                "FRG", "FORGES", "FRGS", "FORK", "FRK", "FORKS", "FRKS", "FORT", "FT",
                                "FRT", "FREEWAY", "FWY", "FREEWY", "FRWAY", "FRWY", "GARDEN", "GDN",
                                "GARDN", "GRDEN", "GRDN", "GARDENS", "GDNS", "GRDNS", "GATEWAY", "GTWY",
                                "GATEWY", "GATWAY", "GTWAY", "GLEN", "GLN", "GLENS", "GLNS", "GREEN", "GRN",
                                "GREENS", "GRNS", "GROVE", "GRV", "GROV", "GROVES", "GRVS", "HARBOR", "HBR",
                                "HARB", "HARBR", "HRBOR", "HARBORS", "HBRS", "HAVEN", "HVN", "HEIGHTS",
                                "HTS", "HIGHWY", "HIWAY", "HIWY", "HWAY", "HILL",
                                "HL", "HILLS", "HLS", "HOLLOW", "HOLW", "HLLW", "HOLLOWS", "HOLWS",
                                "INLET", "INLT", "ISLAND", "IS", "ISLND", "ISLANDS", "ISS", "ISLNDS",
                                "ISLE", "ISLES", "JUNCTION", "JCT", "JCTION", "JCTN",
                                "JUNCTN", "JUNCTON", "JUNCTIONS", "JCTS", "JCTNS", "KEY", "KY", "KEYS",
                                "KYS", "KNOLL", "KNL", "KNOL", "KNOLLS", "KNLS", "LAKE", "LK", "LAKES",
                                "LKS", "LAND", "LANDING", "LNDG", "LNDNG", "LIGHT",
                                "LGT", "LIGHTS", "LGTS", "LOAF", "LF", "LOCK", "LCK", "LOCKS", "LCKS",
                                "LODGE", "LDG", "LDGE", "LODG", "LOOP","LOOPS", "MALL", 
                                "MANOR", "MNR", "MANORS", "MNRS", "MEADOW", "MDW", "MEADOWS", "MDWS",
                                "MEDOWS", "MEWS", "MILL", "ML", "MILLS", "MLS", "MISSION", "MSN",
                                "MSSN", "MISSN", "MOTORWAY", "MTWY", "MOUNT", "MT", "MNT", "MOUNTAIN",
                                "MTN", "MNTAIN", "MNTN", "MOUNTIN", "MTIN", "MOUNTAINS", "MTNS", "MNTNS",
                                "NECK", "NCK", "ORCHARD", "ORCH", "ORCHRD", "OVAL",  "OVL", "OVERPASS",
                                "OPAS", "PARK", "PRK", "PARKS",
                                "PARKWY", "PKWAY", "PKY", "PARKWAYS", "PKWYS", "PASS",
                                "PASSAGE", "PSGE", "PATHS", "PIKE",  "PIKES",
                                "PINE", "PNE", "PINES", "PNES", "PLACE", "PL", "PLAIN", "PLN", "PLAINS",
                                "PLNS", "PLAZA", "PLZ", "PLZA", "POINT", "PT", "POINTS", "PTS", "PORT",
                                "PRT", "PORTS", "PRTS", "PRAIRIE", "PR", "PRR", "RADIAL", "RADL", "RAD",
                                "RADIEL", "RAMP", "RANCH", "RNCH", "RANCHES", "RNCHS", "RAPID",
                                "RPD", "RAPIDS", "RPDS", "REST", "RST", "RIDGE", "RDG", "RDGE",
                                "RIDGES", "RDGS", "RIVER", "RIV", "RVR", "RIVR",
                                "ROADS", "RDS", "ROUTE", "RTE", "ROW", "RUE",
                                "RUN", "SHOAL", "SHL", "SHOALS", "SHLS", "SHORE", "SHR",
                                "SHOAR", "SHORES", "SHRS", "SHOARS", "SKYWAY", "SKWY", "SPRING", "SPG",
                                "SPNG", "SPRNG", "SPRINGS", "SPGS", "SPNGS", "SPRNGS", "SPUR",
                                "SPURS", "SPUR", "SQUARE", "SQ", "SQR", "SQRE", "SQU", "SQUARES",
                                "SQS", "SQRS", "STATION", "STA", "STATN", "STN", "STRAVENUE", "STRA",
                                "STRAV", "STRAVEN", "STRAVN", "STRVN", "STRVNUE", "STREAM", "STRM",
                                "STREME", "STRT", "STR", "STREETS", "STS", "SUMMIT",
                                "SMT", "SUMIT", "SUMITT", "TERRACE", "TER", "TERR", "THROUGHWAY", "TRWY",
                                "TRACE", "TRCE", "TRACES", "TRACK", "TRAK", "TRACKS", "TRK", "TRKS",
                                "TRAFFICWAY", "TRFY", "TRAIL", "TRL", "TRAILS", "TRLS", "TRAILER",
                                "TRLR", "TRLRS", "TUNNEL", "TUNL", "TUNEL", "TUNLS", "TUNNELS",
                                "TUNNL", "TURNPIKE", "TPKE", "TRNPK", "TURNPK", "UNDERPASS", "UPAS",
                                "UNION", "UN", "UNIONS", "UNS", "VALLEY", "VLY", "VALLY",
                                "VLLY", "VALLEYS", "VLYS", "VIADUCT", "VIA", "VDCT", "VIADCT",
                                "VIEW", "VW", "VIEWS", "VWS", "VILLAGE", "VLG",
                                "VILL", "VILLAG", "VILLG", "VILLIAGE", "VILLAGES", "VLGS", "VILLE",
                                "VL", "VISTA", "VIS", "VIST", "VST", "VSTA", "WALK", "WALK",
                                "WALKS", "WALL", "WAY", "WAYS",
                                "WELL", "WL", "WELLS", "WLS", 
                                // Spanish street suffixes
                                "CAMINITO", "CMT", "ENTRADA", "ENT",
                                "PLACITA", "PLA", "RANCHO", "RCH"
		
};

#endif
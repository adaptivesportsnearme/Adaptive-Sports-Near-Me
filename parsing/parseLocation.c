/*
    This is an address parsing module, designed to extract location fields
    from unformatted strings.

    When a human is given the input

        732849 California Ct.
        Washington, NJ 732848
        USA

    It is fairly easy to parse that information into these fields:

        House number: 732849
        Street: California Ct (Court)
        City: Washington
        State: New Jersey
        ZIP: 732848
        Country: USA

    But that relies a lot on human brain power. For instance, the house number and
    zip code are nearly the same, and California, Ct, Washington, and NJ could all
    refer to states. This module aims to replicate how the human brain leverages information
    position and standard formatting to parse this information
*/

/* 
    In multiple places within this code, the term multistate (or multicity) is used.
    A multistate denotes a state, territory, or province and contains spaces

    For instance "New Jersey", "District of Columbia", and "Northwest Territories" 
    are all multistates, but "Alabama", "DC", and "NT" are not, even though
    "DC" and "NT" refer to the above states that use spaces. The reason for this
    discrepancy is that this is a programming-related label that will not be used outside
    of this module's internal implementation.
*/

/*  ###########################  IMPLEMENTATION GUIDE  ################################

    Hello! Please read this whole thing in full. I've spent something like 10-20 hours accumulating
    the below information, and reading this should help save you from spending so much time on this
    task.

    GENERAL INFORMATION
        When we get a string, we'd like to split it up along its boundaries (whitespaces, commas, 
        probably periods) and determine which segments correspond to different information, and 
        which contain no useful information at all. Once we've labeled what information 
        is what, we'll extract it and pass it to the provided container. 
        
        At its current state there's no such feature, but I'd like to add an informal confidence 
        measure for each field, so we can overwrite weak matches with stronger ones. It would 
        also be nice for the human reviewer. Either way, we'll try to develop a confidence scoring system.

        I think it's best to develop different (private) matching functions and then check the string
        against each of them. That way, we could work on matching different kinds of input, and we wouldn't
        need huge long if-then chains to control for variety in input.


    In this module, we want to extract the following information from a string:

     - STREET ADDRESS
            Like "1234 N. Hollywood Blvd." For now, we want to push both address lines
            into address_line_1. This is the second hardest field to retrieve accurately,
            so here are some principles for parsing this field.
            
             - All street addresses must begin with a house number. The first character
               in this house number must be a digit. However, it is worth noting that not
               all house numbers are strictly digits. 
            
                    Letters are allowed: "221B Baker Street", "221 B Baker Street"
                    Hyphens are allowed: "112-10 Bronx Road"
                    Fractions are allowed: "123-1/2 Alphabet Rd", "123 1/2 Alphabet Rd"
                    Spaces are allowed: "237 42 Frontera Ln."
               
               However, if a house has a letter, hyphen, or fraction in the first segment,
               it cannot be a two-segment address. We're also assuming that a house number can consist
               of no more than two segments.

             - Some street addresses may have directionals : N, E, S, W, NE, ...
               These can generally be considered part of the street name, and are relatively
               safe to ignore

             - Street names are weird and loosely formatted, but we can get some restrictions. We mostly
               just want to make sure their characters are relatively normal. Street names can have spaces,
               numbers, hyphens, commas, apostrophes, periods, and probably more. We do want to make sure
               that there is at least one segment dedicated to a street name, as the minimal possible address
               is a house number and street name.
               
             - Almost always, street names will end in a street suffix. There is a super-comprehensive list of
               these in locationReference.h, which you should use, BUT MAKE SURE YOU INCLUDE THE ATTACHED LICENSE
               IF YOU USE THIS AS I DID NOT COME UP WITH THE LIST!!!!

                ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^  IMPORTANT ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

               A lot of street suffixes will end with a period, which is important, as periods are not included
               in the list in locationReference.h, so you should get rid of trailing periods before checking for
               list inclusion.

               It's also reasonable to require that an address has a street suffix to match, as it is almost
               unambiguously the end of the street address

             - Positionally, street names usually occur first, so if there are multiple candidates, we should 
               take the first, but we should also avoid these kinds of strings being passed.

     - CITY
            This is the hardest field to extract, because it lacks uniform formatting, and
            it's very easy to confuse with other fields (Washington and Canada are both valid
            city names). 

             - US city names can contain digits, commas, hyphens, and periods (usually only
               after certain things, like "St", "Ft", etc.). Canada has a city called "Saint-Louis-du-Ha! Ha!"
               so they're just doing whatever. Just make sure they lead with an alphabetic character. Diacritics
               should be flattened to their ASCII counterparts before being passed, so just worry about their
               ASCII representation.

             - Position is important for this one. City is usually placed behind the street address and right
               before State. It's not unreasonable to assume for now that any segment immediately before state that 
               ends in a comma is the last segment in a city name.

             - Because of how ambiguous it is, we should try to get state and country before city, so we have the
               least risk of accidentally misidentifying it.

             - Also, we can make safer assumptions by knowing a list of common cities. Also in locationReference.h
               there is a list of major US and Canadian cities, with just about every city with a population of
               above 100k people. It's far from comprehensive, but if we get a positive match, then at least we 
               can be pretty certain that we've found the city field. I've also included a list of the first words
               of major multicities, but hopefully python is friendlier in terms of string matching.

     - STATE
            This is one of the friendliest fields to extract, because we can exhaustively list them. Although
            now that I think about it, it would probably be best to add US territories, as they're not currently 
            included in our list of abbreviations.

             - I've included a list of US States and Canadian territories. I've also attached a list of common 
               misspellings, but we should be careful about false positive matches.

             - The only real tricky part here is that a lot of other fields can look like states. For instance
               there are a lot of cities called Washington or Wyoming. There are also street suffixes that can
               look like states, like CT (court), WY (way), or MT (mountain). Finally, there are also streets 
               named after states. As usual, use position as a tiebreaker. States usually go last in a string, 
               but it might also be worth checking that a state name is not preceded by a house number and 
               succeeded by a street suffix

     - Country
            United States, US, and USA are nearly unambiguous state indicators. Canada is a little trickier,
            as I've found both streets and cities in the US named Canada.

             - In the typical mailing address format, country goes last, so use a backwards sweep to find
               country.

             - But in general don't sweat this field too much. In the future we'll probably be storing a default
               country for organizations, and that's probably the safest location field to assume is the same
               across an organization's listings.

             - Honestly, country is excluded so much, it might only be worth checking as the last location field.
               If you're using the same splitting technique, the module calling the function might be inaccurate,
               so it's worth checking more than just the last segment, but we can probably safely assume country
               is never before any other field.

             - Also a confident state match necessarily determines country. So if you know the state is NL
               (Newfoundland and Labrador), then you're definitely in Canada. Similarly, ZIP code formats
               vary by country, so that also determines country.

     - ZIP
            Probably the most unique field, so it's very nice to grab this one first. There are three considerations:
            ZIP, ZIP+4, and the Canadian Postal Code format. The Canadian postal code format is more complex than the US,
            but both are pretty restrictive. 

             - The only potential mixup is for house number. The best approach here is to use position. House
               numbers are usually the first field, and ZIP is usually last or second to last, so choose the last
               one if there's ambiguity.

             - ZIP is nice because it determines country for sure, and usually state, though some ZIP codes cross states.
               Some ZIP codes even determine down to street address, but most of those are for huge corporate headquarters
               or federal facilities, so we're unlikely to see those ZIP codes in practice.

     - TIMEZONE
            I'm still not sure how to tackle timezone, as it's a location-determined field that tells you
            how to interpret time data. I'll just say that timezones are weird. The tz database splits a lot
            of major time zones into smaller parts based on how certain regions legislate time. Each tz timezone
            is a region where all local clocks have agreed since 1970, so it encodes both UTC offset (PST, EST, etc.)
            and how things like daylight savings are handled. Also note that there are some cities that have
            two timezones, so this one isn't always determinable from city, but it usually is.


    OTHER PARSING HEURISTICS
      - No single segment provides information for more than one field, other than
        by dependency
    
      - Information is usually provided in the order of the US Mailing Format:
            [House Number](Optional Directional)[Street Name][Street Suffix](Field separator, like comma or newline)
            [City], [State] [ZIP] (Country)
        And so we should try to always be able to extract information in this format. In other words,
        it's better that we can only retrieve information in this format than being able to retrieve 
        information in all other formats but not this one.

      - Certain fields are likely to be clumped together regardless of format. For the final parser,
        address lines 1 & 2 are likely to be together, and city, state, and ZIP are likely to be clumped.

      - If multiple things could be the same field, the least ambiguous entry should be chosen. For
        instance, imagine we see that both CT and New Hampshire could be state. CT matches as a state or
        a street suffix, but New Hampshire only matches as a state, so we choose New Hampshire as our state.

      - As the previous heuristic indicates, in general each field is represented only once. We should not
        see that there are multiple cities for a single address, or multiple states. If we're actually seeing
        that there are multiple addresses, then that's an indication we should either be looking at only one
        (Like if the event is a bike ride from location A to location B, where since we only care about starting
        location, we choose Location A), or that we're looking at two adaptive sports instances (like on calendar
        pages).

      - Location information is usually presented without any non-location information between. In other words,
        once we see our first location match, we're likely to see only location information until all location
        info has been conveyed

      - Single fields are pretty much never strongly separated (Across html tags, or by a newline), and are rarely
        weakly separated (by commas, for instance). These separators should be leveraged to more accurately divide
        the input, and you'll see there is actually an unused function to identify these separators in a string.


    TESTING GUIDELINES
        For developing test cases, please try the following

          - A lot of standard-format addresses, including those separated by newline. These are the addresses
            that will most often be passed, so it's super important to make sure we can accurately parse these.
            Make sure to test:

              - Long instances in fields -- long city names, long state names, long street names, especially those with 
                common punctuation marks to make sure those don't cause problems

              - Short instances as well -- the first version of the matchAddress function here accidentally
                didn't match for single-word street names, as I was super focused on matching long street
                names accurately

              - Combinations of optional fields (country, street directionals, zip codes)

              - Fields that look like each other -- state names as city and street names, for instance. Make 
                sure we can leverage the standard format to handle these ambiguities.

              - Similar to the above, make sure we test house numbers that look like zip codes, including when there's
                no zip code present.

              - Field mismatches, like US postal codes with a Canadian Territory. Try a couple of these, where
                two fields cannot coexist. But don't worry about nonexistent cities at this point, we won't check
                that all cities are real cities in the given state. Also don't worry about ZIP/State mismatches either.
                Just make sure that mismatches for implied country don't go through.

              - https://www.summet.com/dmsi/html/codesamples/addresses.html also contains a list of standard format 
                addresses. Just note that we don't care about P.O. box or name, since that shouldn't occur as an 
                adaptive sports location.

          - Some single-field info, especially street address. Anything that strongly matches a street address is fine to 
            pass, but avoid false positives ("100 people" should not register as a street address). Edge-case false 
            positives are acceptable though, like "20 Rusty Springs" (since Springs is a street suffix), since those will 
            go through the human review queue. The risk of false positives seems high, but we should not be ignoring
            valuable information like "1203 N Webster St.", which is unambiguously an address. It would be nice to 
            indicate confidence somehow on certain fields, so we could overwrite mismatches like that if we find a 
            more appropriate street address, but that's something I'll elaborate on later.

            The only other single field we should extract is Canadian ZIP. Any other format is too ambiguous for us to
            want to take it. (For instance "10000 - 6000 = 4000" could accidentally match for ZIP+4 10000-6000)

          - Also, selected sequences of fields from the US standard address format, in case one location is passed in
            multiple parts. In particular, sequences of city, state, zip, and country should pass in combination.

          - For negative matches, try a lot of random sentences with numbers in them, or lists. Also try anything
            you can come up with that looks like an address at first but clearly isn't. Assume people aren't putting
            adversarial addresses on their websites, and only choose cases that could appear in the real world. Try
            to put things that could match for certain fields to make sure we're choosing as few false positives as 
            possible. Things like "12 reasons you should never visit Canada" and "Washington, Oregon and California"
            should not match.
        
          - Also check for exceptional strings, like those with multiple newlines in a row, zero-length strings, 
            or super long strings of characters with no spaces. However, string data should be cleaned before being
            sent here, so don't worry about non printable characters except for \r and \n

          - Also, every string that matches as an address should be parseable. It doesn't have to be the other way
            around (it's fine if the parsing function is stronger than the matching function, but we should ideally
            be trying to make them work with the same strings), but it's pretty necessary that anything that matches 
            can be parsed. Thanks for reading all the way through, let me know you did and I will buy you a candy bar.
            In python you can probably put every test case in one text file and set a for loop to make sure that every 
            string that matches can be parsed as well.

    Please let me know if you have any questions on how to transfer this parsing process, and I'll be happy to help.
    You can also refer to my implementation, but please be aware that the parsing function and especially the address-matching
    function was a little bit rushed to complete deadlines, and requires a lot of restrictions.

    I'll also note that when you are passing information into a container, try to assemble all the information first before 
    passing it into the provided container, so there's less risk of strange behavior surrounding crashes or other errors.


*/


#include "parseLocation.h"
#include "locationReference.h"


// ------------- BASIC HELPER FUNCTIONS ---------------------- //


// Hello just ignore these. they're pretty much all default python methods, 
// or otherwise one-liners. For instance the splitString() method is basically 
// just string.split()


// returns true if the strings are equal -- case insensitive
// and false otherwise
int strEquals(char * str1, char * str2) {
    if (strlen(str1) != strlen(str2)) return 0;

    for (int i = 0; i < strlen(str1); i++) {
        if (toupper(*(str1 + i)) != toupper(*(str2 + i))) return 0;
    }

    return 1;
}

// Taking the given sequence of num words starting at index in arr,
// returns a string pointer that concatenates the words with a joining space.
// NO SAFEGUARDS FOR MEMORY BUFFER INVALID READ -- do a check before passing
char * strCombineSpace(char ** arr, int index, int num) {
    int len = 0;

    // count space needed
    for (int i = 0; i < num; i++) {
        len += strlen(*(arr + index + i));
    }

    // len characters + (num - 1 spaces) + null character
    char* ret = calloc(len + num, sizeof(char));

    len = 0;
    for (int i = 0; i < num; i++) {
        memcpy(ret + len, *(arr + index + i), strlen(*(arr + index + i)));
        len += strlen(*(arr + index + i)) + 1;
        if (i != num - 1) {
            *(ret + len - 1) = ' ';
        }
    }

    return ret;
}

// returns 1 if str is an entry in arr, and 0 otherwise
int strInArr(char * str, char** arr, int array_size) {

    for (int i = 0; i < array_size; i++) {
        if (strEquals(str, (char *)(*(arr + i)))) return 1;
    }

    return 0;
}

// finds the index of str in arr, or returns -1 if str is not in arr
int strFindInArr(char * str, char** arr, int size) {
    for (int i = 0; i < size; i++) {
        if (strEquals(str, *(arr + i))) return i;
    }

    return -1;
}

// returns 1 if a string is only digits, and 0 otherwise
int strIsDigit(char * str) {
    for (int i = 0; i < strlen(str); i++) {
        if (!isdigit(*(str + i))) return 0;
    }
    return 1;
}

// Given an array and a string, returns the substring on str that
// corresponds to a given series of (len) segments in splitString(str)
// which start at start_index
char * strBuildFromArr(char* str, int start_index, int len) {
    
    if (str == NULL) return NULL;

    // an integer corresponding to an index in splitString(str)
    int index = 0;
    
    // the start of our substring
    int start = 0;

    // advances start to the index of the first non-space character in the string.
    while (start < strlen(str) && (isspace(*(str + start)) || *(str + start) == ',')) {
        start++;
    }

    if (start == strlen(str)) return NULL; // returns if passed a string with only spaces and commas

    // keeps track of if the previous character was a space or comma
    int lastwasspace = 0;


    while (index < start_index) {
        start++;
        if (start >= strlen(str)) return NULL; // return if out of range
        if (isspace(*(str + start)) || *(str + start) == ',') {
            lastwasspace = 1;
        } else {
            if (lastwasspace == 1) {
                index++;
            }
            lastwasspace = 0;
        }
    }

    // start is now at the start of our desired substring, and index = start_index
    // we now find the end of our desired substring

    // the end of our substring
    int end = start;

    // note that index here will change on the shift to whitespace instead of the
    // reverse shift. This is so we don't include unnecessary spaces or commas

    while (index < start_index + len) {
        end++;
        if (end >= strlen(str)) return NULL;
        if (isspace(*(str + end)) || *(str + end) == ',') {
            if (lastwasspace == 0) {
                index++;
            }
            lastwasspace = 1;
        } else {
            lastwasspace = 0;
        }
    }



    // start and end are now at the right places.
    // start is at the first included index
    // end is at the first excluded index

    char * ret = calloc(end - start + 1, sizeof(char));
    memcpy(ret, str + start, end - start);


    return ret;
    
}


// Given an array and a string, returns the substring index on str that
// corresponds to the given index in a splitString array
int strIndexFromArr(char* str, int arr_index) {
    
    if (str == NULL) return -1;

    // an integer corresponding to an index in splitString(str)
    int index = 0;
    
    // the start of our substring
    int start = 0;

    // advances start to the index of the first non-space character in the string.
    while (start < strlen(str) && (isspace(*(str + start)) || *(str + start) == ',')) {
        start++;
    }

    if (start == strlen(str)) return -1; // returns if passed a string with only spaces and commas

    // keeps track of if the previous character was a space or comma
    int lastwasspace = 0;


    while (index < arr_index) {
        start++;
        if (start >= strlen(str)) return -1; // return if out of range
        if (isspace(*(str + start)) || *(str + start) == ',') {
            lastwasspace = 1;
        } else {
            if (lastwasspace == 1) {
                index++;
            }
            lastwasspace = 0;
        }
    }

    // start is now at the start of our desired substring, and index = start_index

    return start;
    
}


// Given a splitString array and its corresponding string, returns the index that is the
// first after a newline or comma. An example of output depending on input index is given below
/*        2   2   4    4     6   6      8      8
        "This is, a string, with a \nnewline break",
*/
int getNextBreakInArr(char* str, int start_index, int size) {
    
    if (str == NULL) return -1;

    // an integer corresponding to an index in splitString(str)
    int index = 0;
    
    // the start of our substring
    int letter = 0;

    // advances start to the index of the first non-space character in the string.
    while (letter < strlen(str) && (isspace(*(str + letter)) || *(str + letter) == ',')) {
        letter++;
    }

    if (letter == strlen(str)) return -1; // returns if passed a string with only spaces and commas

    // keeps track of if the previous character was a space or comma
    int lastwasspace = 0;

    while (index < start_index) {
        letter++;
        if (letter >= strlen(str)) return -1; // return if out of range
        if (isspace(*(str + letter)) || *(str + letter) == ',') {
            lastwasspace = 1;
        } else {
            if (lastwasspace == 1) {
                index++;
            }
            lastwasspace = 0;
        }
    }

    // letter is now at the start of our desired substring, and index = letter
    // we now find the next index after a newline or comma

    // note that index here will change on the shift to whitespace instead of the
    // reverse shift. This is so we don't include unnecessary spaces or commas

    while (index < size) {
        letter++;
        if (letter >= strlen(str)) {
            if (index == size - 1) return size;
            return -1;
        }
        if (isspace(*(str + letter)) || *(str + letter) == ',') {
            if (lastwasspace == 0) {
                index++;
            }
            if (*(str + letter) == ',' || *(str + letter) == '\n') { // return index if we see a comma or newline
                return index;
            }
            lastwasspace = 1;
        } else {
            lastwasspace = 0;
        }
    }

    return index;
    
}

// clears all entries in a string array with ct entries
void clearStrArr(char ** arr, int ct) {
    for (int i = 0; i < ct; i++) {
        if (*(arr + i) != NULL) free(*(arr + i));
    }
}



// Given an array of strings, a starting index, and the size of the array,
// If the index is the first of n entries that comprise a US/CA postal code,
// then the function will return n. Otherwise it will return 0
int matchZip(char ** arr, int index, int size) {
    // matches 12345 and 12345 6789
    if (strlen(*(arr + index)) == 5 && strIsDigit(*(arr + index))) {
        if (index < size - 1) {
            // input is ZIP+4, separated by a space
            if (strlen(*(arr + index + 1)) == 4 && strIsDigit(*(arr + index + 1))) return 2;
        }
        return 1;
    }
        // matches 12345-6789
    if (strlen(*(arr + index)) == 10) {
        if (*(*(arr + index) + 5) != '-') return 0;
        for (int i = 0; i < 5; i++) if (!isdigit(*(*(arr + index) + i))) return 0;
        for (int i = 6; i < 10; i++) if (!isdigit(*(*(arr + index) + i))) return 0;
        return 1; // input is ZIP+4, separated by a hyphen
    }
        // matches Canadian postal code -> [ABCEGHJ-NPRSTV-Y]\d[ABCEGHJ-NPRSTV-Z]\d[ABCEGHJ-NPRSTV-Z]\d
    if (strlen(*(arr + index)) == 6) {
        if (!isdigit(*(*(arr + index) + 1)) || !isdigit(*(*(arr + index) + 3)) || !isdigit(*(*(arr + index) + 5))) return 0;
        if (!isalpha(**(arr + index)) || tolower(**(arr + index)) == 'd' || tolower(**(arr + index)) == 'f'
            || tolower(**(arr + index)) == 'i' || tolower(**(arr + index)) == 'o' || tolower(**(arr + index)) == 'q'
            || tolower(**(arr + index)) == 'u' || tolower(**(arr + index)) == 'z') return 0;
        if (!isalpha(*(*(arr + index) + 2)) || tolower(*(*(arr + index) + 2)) == 'd' || tolower(*(*(arr + index) + 2)) == 'f'
            || tolower(*(*(arr + index) + 2)) == 'i' || tolower(*(*(arr + index) + 2)) == 'o' || tolower(*(*(arr + index) + 2)) == 'q'
            || tolower(*(*(arr + index) + 2)) == 'u') return 0;
        if (!isalpha(*(*(arr + index) + 4)) || tolower(*(*(arr + index) + 4)) == 'd' || tolower(*(*(arr + index) + 4)) == 'f'
            || tolower(*(*(arr + index) + 4)) == 'i' || tolower(*(*(arr + index) + 4)) == 'o' || tolower(*(*(arr + index) + 4)) == 'q'
            || tolower(*(*(arr + index) + 4)) == 'u') return 0;
        return 1; // input is CA postal code
    }
        // matches Canadian postal code -> [ABCEGHJ-NPRSTV-Y]\d[ABCEGHJ-NPRSTV-Z][ ]\d[ABCEGHJ-NPRSTV-Z]\d
    if (strlen(*(arr + index)) == 3) {
        if (!isdigit(*(*(arr + index) + 1))) return 0;
        if (!isalpha(**(arr + index)) || tolower(**(arr + index)) == 'd' || tolower(**(arr + index)) == 'f'
            || tolower(**(arr + index)) == 'i' || tolower(**(arr + index)) == 'o' || tolower(**(arr + index)) == 'q'
            || tolower(**(arr + index)) == 'u' || tolower(**(arr + index)) == 'z') return 0;
        if (!isalpha(*(*(arr + index) + 2)) || tolower(*(*(arr + index) + 2)) == 'd' || tolower(*(*(arr + index) + 2)) == 'f'
            || tolower(*(*(arr + index) + 2)) == 'i' || tolower(*(*(arr + index) + 2)) == 'o' || tolower(*(*(arr + index) + 2)) == 'q'
            || tolower(*(*(arr + index) + 2)) == 'u') return 0;
        if (index < size - 1) {
            if (strlen(*(arr + index + 1)) == 3) {
                if (!isdigit(**(arr + index + 1)) || !isdigit(*(*(arr + index + 1) + 2))) return 0;
                if (!isalpha(*(*(arr + index + 1) + 1)) || tolower(*(*(arr + index + 1) + 1)) == 'd' || tolower(*(*(arr + index + 1) + 1)) == 'f'
                    || tolower(*(*(arr + index + 1) + 1)) == 'i' || tolower(*(*(arr + index + 1) + 1)) == 'o' || tolower(*(*(arr + index + 1) + 1)) == 'q'
                    || tolower(*(*(arr + index + 1) + 1)) == 'u') return 0;
                return 2;
            }
            return 0;
            
        }     
        return 0; 
    }

    return 0;
}


// Given an array of strings, a starting index, and the size of the array,
// if the string at the index is the first of n entries that comprise a US state or 
// Canadian territory or province, then the function will return n. 
// Otherwise, it will return 0.

// this only matches spellings identical to common names.
int matchStateStrong(char** arr, int index, int size) {
    // filters out obvious non-matches
    if (strlen(*(arr + index)) < 2) return 0;
    if (strlen(*(arr + index)) == 3 && !strEquals(*(arr + index), "NEW")) return 0;

    // If input is 2-letter state code (CT, NV, DC, NL)
    if (strlen(*(arr + index)) == 2) {
        for (int i = 0; i < (2 * PARLOCNUMSTATES); i += 2) { // only scans even indices -- the only ones that are 2 letters
            if (strEquals(*(arr + index), (char*)*(states + i))) return 1;
        }
        return 0;
    }

    // At this point the only possible input is a full state name

    // index in the multistateCue array
    int msInd = strFindInArr(*(arr + index), (char**)multistateCue, PARLOCNUMMULTISTATES); 

    if (msInd != -1 && (*(multistateCueLength + msInd) + index - 1 < size)) {
        char * multistate = strCombineSpace(arr, index, *(multistateCueLength + msInd)); // ALLOCATES MEMORY
        for (int i = 1; i < (2 * PARLOCNUMSTATES); i += 2) {
            if (strEquals(multistate, (char*)*(states + i))) {
                free(multistate);
                return *(multistateCueLength + msInd);
            }
        }
        free(multistate);
        return 0;
    }

    for (int i = 1; i < (2 * PARLOCNUMSTATES); i += 2) {
            if (strEquals(*(arr + index), (char*)*(states + i))) return 1; // only scans unabbreviated state names
    }
    return 0;

}


// Given an array of strings, a starting index, and the size of the array,
// If the index is the first of some number of entries that comprise a US/CA postal code,
// then the function will return 1. 
// Otherwise it will return 0
int matchStreetAddress(char ** arr, int index, int size) {

    // All valid addresses must begin with a number. This number may include 
    // fractions or decimals and may be multiple segments.
    // The allowed characters for this first field are 0-9, "-", "/", ".", and " "
    // And a single* letter character may indicate unit
    // slashes can only occur after a hyphen or space
    // a digit must begin the first segment

    // * there could probably be multiple letters, but we'd risk false positives
    // for a minor edge case.

    // there cannot be a 1-length address.
    if (index == size - 1) return 0;

    // the number of segments the address number takes up
    int houseNumSegs = 0; 

    // a flag to check if a slash is a violation
    int slashesAllowed = 0;
    // a flag to check if an address number could be in two parts
    int multiNum = 1;

    // if input does not begin with digit we return false
    if (strlen(*(arr + index)) > 0) {
        if (!isdigit(**(arr + index))) return 0;
    }

    for (int i = 1; i < strlen(*(arr + index)); i++) {
        if (!isdigit(*(*(arr + index) + i))) {
            multiNum = 0; // if the first segment has any non-digit characters, it cannot be
                          // part of a multiple-segment address number -- at least under our model
            if (*(*(arr + index) + i) == '-') slashesAllowed = 1;
            else if (*(*(arr + index) + i) == '/') {
                if (!slashesAllowed) return 0;
            }
            else if (*(*(arr + index) + i) != '.') {
                if (isalpha(*(*(arr + index) + i))) {
                    if (i != strlen(*(arr + index)) - 1) return 0;
                }
            }
        }
    }

    houseNumSegs = 1;
    if (multiNum = 1 && index < size - 2) {

        /*  Note that here we don't check if the first character is a digit,
            because addresses like 221 B baker street are fine.

            But what about ambiguous addresses like 404 E Hastings Blvd?
            Here E could be a directional or a unit. 
            
            It turns out this doesn't actually matter. 
            Our algorithm will mark E as a unit number, but
            this marking never reaches the user. Our algorithm will then determine
            that Hastings is not a directional, and we will still mark the
            correct amount of space, despite the potential mis-marking of E.

            If you have something like 221 B E Hastings Blvd., that's very annoying to read
            but we will still interpret B as a unit number and then E as a directional, so no 
            miscounting ever occurs. Cool!
        */

        for (int i = 0; i < strlen(*(arr + index + 1)); i++) {
            if (!isdigit(*(*(arr + index + 1) + i)) && *(*(arr + index + 1) + i) != '-' 
                && *(*(arr + index + 1) + i) != '/' && *(*(arr + index + 1) + i) != '.'
                && !(isalpha(*(*(arr + index + 1) + i)) && i == strlen(*(arr + index + 1)) - 1)) {
                    multiNum = 0; // mark format as invalid
                    break;
            }
        }
        if (multiNum == 1) houseNumSegs = 2;
    }



    // As long as there is a field after the house number, this counts as a match.
    // This is a very weak matching function, so bear that in mind.
    // I have deleted sections from my initial implementation, but I have
    // left some utilities in the locationReference header if you need to
    // parse the street address more precisely.

    if (index + houseNumSegs < size) {
        for (int i = 0; i < strlen(*(arr + index + houseNumSegs)); i++) {
            if (isalpha(*(*(arr + index + houseNumSegs) + i))) return 1; // street name must have at least one alphabetical character
        }
    }

    return 0;
}

// if there is a series of segments that are a major US/CA city, we return
// the size of the largest series starting at index in arr. If the segment
// does not match, we return 0.
// because some cities like Buffalo are contained in other cities like Wood Buffalo
// scanning from R->L can return an early result, so use caution and prefer L-> scans
int matchCommonCity(char ** arr, int index, int size) {
    char * multi;
    if (index <= size - 3) {
        if (strInArr(*(arr + index), (char**)multiCityThreeCues, PARLOCMULTTHRCITYCUES)) {
            multi = strCombineSpace(arr, index, 3);
            if (strInArr(multi, (char**)commonCities, PARLOCCITIES)) {
                free(multi);
                return 3;
            }
        }
    }
    if (index <= size - 2) {
        if (strInArr(*(arr + index), (char**)multiCityThreeCues, PARLOCMULTWOCITYCUES)) {
            multi = strCombineSpace(arr, index, 2);
            if (strInArr(multi, (char**)commonCities, PARLOCCITIES)) {
                free(multi);
                return 2;
            }
        }
    }

    if (strInArr(*(arr + index), (char**)commonCities, PARLOCCITIES)) {
        return 1;
    }
}

// if there is a series of segments in a valid city name format, we return
// the size of the largest series starting at index in arr. If the first segment
// does not match the format, we return 0
int matchCity(char ** arr, int index, int size) {

    int count = 0;

    // longest us city name in terms of word count is "Kinney and Gourlays Improved City Plat"
    // barring official names like "La Villa Real de la Santa Fe de San Francisco de Asis" -- which is Santa Fe
    while (index + count < size && count < 6) {
        for (int i = 0; i < strlen(*(arr + index + count)); i++) {
            if (!isalpha(*(*(arr + index + count) + i)) && *(*(arr + index + count) + i) != '-'
                && *(*(arr + index + count) + i) != '.') {
                    return count;
            }
        }

        count++;
    }
}

int matchCountry(char ** arr, int index, int size) {
    // filters out obvious non-matches
    if (strlen(*(arr + index)) != 2 && strlen(*(arr + index)) != 3 && strlen(*(arr + index)) != 6
                && strlen(*(arr + index)) != 7 && strlen(*(arr + index)) != 13
                && strlen(*(arr + index)) != 24) return 0;

    char * country = NULL;
    int len = 1;

    if (strEquals(*(arr + index), "UNITED")) {

        if (index + 1 >= size) return 0;
        else {
            if (index + 3 < size) {
                if (strEquals(*(arr + index + 2), "OF") && strEquals(*(arr + index + 3), "AMERICA")) {
                    country = strCombineSpace(arr, index, 4);
                    len = 4;
                }
            } 
            if (country == NULL) {
                country = strCombineSpace(arr, index, 2);
                len = 2;
            }
        }
    } else {
        country = calloc(strlen(*(arr + index)) + 1, sizeof(char));
        memcpy(country, *(arr + index), strlen(*(arr + index)));
    }

    if (strInArr(country, (char **  )countries, PARLOCCOUNTRIES)) {
        free(country);
        return len;
    } else {
        free(country);
        return 0;
    }
}

// if the given array has a street suffix at the given index, we return 1
// otherwise we return 0
int matchStreetSuffix(char ** arr, int index, int size) {
    char * suff;
    // iterate backwards from the last possible street segment
    if (*(*(arr + index) + strlen(*(arr + index)) - 1) == '.') {
        suff = calloc(strlen(*(arr + index)), sizeof(char));
        memcpy(suff, *(arr + index), strlen(*(arr + index)) - 1 );
    } else {
        suff = calloc(strlen(*(arr + index)) + 1, sizeof(char));
        memcpy(suff, *(arr + index), strlen(*(arr + index)));
    }
    int result = strInArr(suff, (char**)streetSuffixes, PARLOCSTRSUFS);
    free(suff);
    return result;
}


// Returns the canonical form of a given state in new memory
// INPUT MUST BE A MATCHING ZIP, or in other words,
// matchZip(arr, index, num) > 0. If this condition is not met,
// strange behavior may occur.
char * getCanonicalZip(char** arr, int index, int num) {
    if (num == 1) {
        char * ret = calloc(strlen(*(arr + index)) + 1, sizeof(char));
        memcpy(ret, *(arr + index), strlen(*(arr + index)));
        return ret;
    } else {
        if (num == 2 && strlen(*(arr + index)) == 3) { // if input is CA postal code in XXX XXX format
            char * ret = calloc(7, sizeof(char));
            memcpy(ret, *(arr + index), 3);
            memcpy(ret + 3, *(arr + index + 1), 3);
            return ret;
        }
        if (num == 2 && strlen(*(arr + index)) == 5) {
            char * ret = calloc(11, sizeof(char));
            memcpy(ret, *(arr + index), 5);
            *(ret + 5) = '-';
            memcpy(ret + 6, *(arr + index + 1), 4);
            return ret;
        }
        return NULL; // YOU SHOULD NEVER REACH THIS LINE OF CODE
    }

}

// Returns the canonical form of a given state in new memory
// INPUT MUST BE A MATCHING STATE, or in other words,
// matchStateStrong(arr, index, num) > 0. If this condition is not met,
// strange behavior may occur:
    // If the input is already two characters, then it will return those two 
    // characters in new memory REGARDLESS OF IF THEY REPRESENT A STATE.
    // Otherwise NULL will be returned if we do not reach out of bounds
char * getCanonicalState(char ** arr, int index, int num) {

    // If the input is two characters, a copy will be returned in new memory
    if (strlen(*(arr + index)) == 2) {
        char * ret = calloc(3, sizeof(char));
        memcpy(ret, *(arr + index), 2);
        if (islower(*ret)) *ret = toupper(*ret);
        if (islower(*(ret + 1))) *(ret + 1) = toupper(*(ret + 1));
        return ret;
    }

    // handles multistates
    if (num > 1) {
        char * multistate = strCombineSpace(arr, index, num); // ALLOCATES MEMORY
        for (int i = 1; i < (2 * PARLOCNUMSTATES); i += 2) {
            if (strEquals(multistate, (char*)*(states + i))) {
                char * ret = calloc(3, sizeof(char));
                memcpy(ret, (char*)*(states + i - 1), 2);
                free(multistate);
                return ret;
            }
        }
        free(multistate);
        return NULL;
    }
}

// Returns the canonical form of a given country in new memory
// INPUT MUST BE A MATCHING COUNTRY, or in other words,
// matchCountry(arr, index, num) > 0. If this condition is not met,
// strange behavior may occur:
char * getCanonicalCountry(char ** arr, int index, int num) {
    char * combined = strCombineSpace(arr, index, num);
    int posInCntry = strFindInArr(combined, (char**)countries, PARLOCCOUNTRIES);
    char * ret;
    if (posInCntry == 4) {
        ret = calloc(3, sizeof(char));
        sprintf(ret, "CA");
    } else {
        ret = calloc(4, sizeof(char));
        sprintf(ret, "USA");
    }

    return ret;
}


/*  A function that takes a given valid c-string str and splits it along
    whitespaces and commas for easier containment, putting it into dst. 
    Returns the number of pieces in the array, or 0 if the string
    has no non-space characters or is NULL*/
int splitString(char*** dst, char * str) {

    if (str == NULL) return 0;

    // The array of strings we will return
    char ** pieces;

    // the number of splits we will make. This excludes whitespace and commas at 
    // the start and end of a string
    int spaces = 0;
    
    int start = 0;

    // advances start to the index of the first non-space character in the string.
    while (start < strlen(str) && (isspace(*(str + start)) || *(str + start) == ',')) {
        start++;
    }

    if (start == strlen(str)) return 0; // returns if passed a string with only spaces and commas

    // counts the number of segments needed to split the string
    int lastwasspace = 0;
    for (int i = start; i < strlen(str); i++) {
        if (isspace(*(str + i))|| *(str + i) == ',') {
            if (lastwasspace == 0) {
                spaces++; 
            }
            lastwasspace = 1;
        } else lastwasspace = 0;
    }
    if (lastwasspace == 1) spaces--; // if the string ends with a space we don't count it

    pieces = calloc(spaces + 1, sizeof(char*));

    // the segment in the string we are currently copying
    int currpiece = 0;

    // an index to store where we are in the given string
    int sind = start;
    // an index to find the end of a given segment
    int eind = sind;

    // splits the string into pieces
    while (currpiece < spaces + 1) {

        // places eind at the first space after *(str + sind), or at the null character if it comes first
        while (*(str + eind) != ',' && !isspace(*(str + eind)) && eind < strlen(str)) {
            eind++;
        }

        // copies the ith string segment into *(pieces + i)
        *(pieces + currpiece) = calloc(eind - sind + 1, sizeof(char));
        memcpy(*(pieces + currpiece), str + sind, eind - sind);
        *(*(pieces + currpiece) + eind - sind) = '\0';

        // readies 
        while ((isspace(*(str + eind)) || *(str + eind) == ',') && eind < strlen(str)) {
            eind++;
        }
        sind = eind;

        currpiece++;
    }

    *dst = pieces;
    return spaces + 1;

}


/*
    Formatting strength & info

        ZIP - must be formatted correctly or information lost --------- done!!
            - usually at the end of an address or by the city or state
            - typically determines state
            - format determines country
        State - finite correctly spelled possibilities, but high possibility for misread ------ done!
              - Determines country
              - usually at end of address and by country
              - country will almost always be after
        Street number - number, ideally in front of street name  -- done
                      - no letters unless there's a letter at the end
                      - usually first field in address
                      - usually not mistakeable for other fields except potentially zip
                      - always before street info
        street prefix - very fixed, has to be direction (can be abbreviated or in spanish though) -- N/A
                      - smallest number of possibilities, but can be a multi-cue for a state
                      - Right after street number if present and right before street
        Street name - Loosest field -- N/A
                    - usually towards start
                    - after street prefix or street number
                    - before street suffix if suffix is present
        Street suffix - wide array of possibilities but still limited -- N/A
                      - can be spanish or english
                      - after street name always
                      - can be omitted
        City - technically limited but often from limited pool
                -> Chicago easily recognizable as city but maybe not Adaven
             - usually before state and after street info
             - technically often obeys certain naming practices
        Country - often omitted
                - in current model, determined by state
                - also determined by zip code
                - if present, likely at the end

    Other formatting heuristics:
        - No segment of text can provide information for two fields other than by determination
        - Information usually goes specific to unspecific
        - information is often clumped
            - City, State, (Country), ZIP usually together at end
            - Street info usually together at start
            - City and State usually clumped in middle
        - if multiple segments could be the same field, the segment with
          the least amount of confusion is likely that field
        - No field is represented more than once in general
        - Address information is usually all in a row 
            -> very rarely will there be irrelevant information in between
               the address information
        - A single field is usually not split by a newline

*/




/*
    Improvements -> always assumes city starts after suffix
    Improve city match with common cities list or common prefixes (San, Saint, Fort)
    Do better way of finding address maybe
    Use line breaks for field splitting -> as other boundary
*/



// taking a string as input, parses a location into the appropriate fields
// and passes that information into location. If there is an error or
// the string does not have enough information, we will return, leaving 
// location unchanged
void parseLocationASLoc(ASLocation * location, char * str) {

    // Some fields to store data before we push it to location
            // The address
            char * addr;
            // the index the house number starts at in the string, 
            // and the length of the substring corresponding to the house number
            int addr_index = -1, addr_len = 0;

            int suffix_index = -1; // suffix length always 1

            char * city;
            int city_index, city_len = 0;

            char * state;
            int state_index, state_len = 0;

            // the four digit zip code extension if provided.
            char * zip;
            int zip_index, zip_len = 0;

            char * country;
            int country_index, country_len = 0;

    
    char ** arr;
    
    int size = splitString(&arr, str);

    if (size < 3) return;

    // a binary buffer to store which fields we have already parsed
    short * occupied = calloc(size, sizeof(short));

    // find zip code, state and country first, moving R -> L and block it off
    for (int i = size - 1; i >= 0; i--) {
        if (zip_len == 0) {
            int zipRes = matchZip(arr, i, size);
            if (zipRes) {
                zip_index = i;
                zip_len = zipRes;
                for (int j = 0; j < zipRes; j++) {
                    *(occupied + i + j) = 1;
                }
                if (country_len != 0 && state_len != 0) break;
            }
        }
        if (country_len == 0) {
            int countryRes = matchCountry(arr, i, size);
            if (countryRes) {
                country_index = i;
                country_len = countryRes;
                for (int j = 0; j < countryRes; j++) {
                    *(occupied + i + j) = 1;
                }
                if (zip_len != 0 && state_len != 0) break;
            }
        }
        if (state_len == 0) {
            int stateRes = matchStateStrong(arr, i, size);
            if (stateRes) {
                state_index = i;
                state_len = stateRes;
                for (int j = 0; j < stateRes; j++) {
                    *(occupied + i + j) = 1;
                }
                if (country_len != 0 && zip_len != 0) break;
            }
        }
    }

    if (state_len == 0) {
        free(occupied);
        clearStrArr(arr, size);
        free(arr);
        return;
    }

    int nextocc = 0; // storing the next occupied segment, so we can avoid unnecessary checking and overlaps.
    while (*(occupied + nextocc) == 0 && nextocc < size - 1) nextocc++;

    for (int i = 0; i < size; i++) {
        if (*(occupied + i) == 1) continue; // segment already accounted for
        if (i > nextocc) {  // advance nextocc if we have passed it
            while (*(occupied + nextocc) == 0 && nextocc < size - 1) nextocc++;
        }
        if (addr_index == -1) {
            if (matchStreetAddress(arr, i, nextocc)) {
                addr_index = i;
                // iterate backwards from the last possible street segment
                for (int j = nextocc - 1; j > i + 1; j--) {
                    if (matchStreetSuffix(arr, j, nextocc)) {
                        if (j - addr_index + 1 <= 7 && state_index - j + 1 <= 6 ) {
                            suffix_index = j;
                            break;
                        }
                    }
                }
            }
        }
    }

    // RESTRICTION -- EXPAND LATER
    // only allows for Street City State format as a substring
    if (suffix_index != -1 && state_len != 0) {

        if (suffix_index > state_index) { // city field cannot have length > 7
            free(occupied);
            clearStrArr(arr, size);
            free(arr);
            return; // EXIT if we flout this format
        }


        for (int i = addr_index; i < state_index; i++) {
            if (*(occupied + i) != 0) {
                free(occupied);
                clearStrArr(arr, size);
                free(arr);
                return; // EXIT if we flout this format
            }
        }

        addr_len = suffix_index - addr_index + 1;
        city_index = suffix_index + 1;
        city_len = state_index - suffix_index - 1;
        if (city_len < 1) {
            free(occupied);
            clearStrArr(arr, size);
            free(arr);
            return;
        }
    } else {
        free(occupied);
        clearStrArr(arr, size);
        free(arr);
        return; // EXIT IF NOT IN RESTRICTED FORMAT
    }

    state = getCanonicalState(arr, state_index, state_len);

    city = strBuildFromArr(str, city_index, city_len);

    addr = strBuildFromArr(str, addr_index, addr_len);

    if (zip_len != 0) zip = getCanonicalZip(arr, zip_index, zip_len);

    if (country_len != 0) country = getCanonicalCountry(arr, country_index, country_len);

    if (location->city != NULL) {
        free(location->city);
    }
    location->city = city;

    if (location->state != NULL) {
        free(location->state);
    }
    location->state = state;

    if (location->address_line_1 != NULL) {
        free(location->address_line_1);
    }

    location->address_line_1 = addr;

    if (zip_len != 0) {
        if (location->zip != NULL) {
            free(location->zip);
        }
        location->zip = zip;
    }

    if (country_len != 0) {
        if (location->country != NULL) {
            free(location->country);
        }
        location->country = country;
    }

    for (int i = 0; i < size; i++) {
        free(*(arr + i));
    }
    free(arr);
    free(occupied);
}


/*  Takes a string as input and populates the provided ASInstance with the
    location information the string contains. Unknown fields do not overwrite
    what is already stored in the struct. If the provided ASInstance is NULL,
    the function will return. 
    
    The string should be able to convey information to a human about the
    location of an event, and contain little else. Strings containing irrelevant
    information or lacking necessary location information may cause 
    erroneous information to be returned.*/
void parseLocation(ASInstance * inst, char * str) {
    ASLocation * loc = calloc(1, sizeof(ASLocation));

    parseLocationASLoc(loc, str);

    pushASLocation(inst, loc);

    free(loc);
}


/*  Given a string as input, returns 1 if the string contains an address,
    and 0 otherwise. The bounds of where the address matches is put into
    start_index and end_index, but the actual address may exist outside 
    these bounds in practice
    
    For an address to be valid, we need:
        - a street address
        - a street suffix
        - a city
        - a state
    
    We'll assume that we always lead with a street address, so we're looking for a pattern
    where we have (houseNumber)(StreetName)(StreetSuffix)(city)(state) where each parenthesized field has
    length >= 1, and there are no gaps between fields.*/ 
int matchAddress(char * str, int * start_index, int * end_index) {
    char ** arr = NULL;

    int count = splitString(&arr, str);
    if (arr == NULL) return 0;
    if (count < 5) {
        clearStrArr(arr, count);
        free(arr);
        return -1;
    }

    int start; // starting index bound of our search
    int end; // ending index bound of our search

    for (start = 0; start < count - 5; start++) {
        // scroll until we find the start of an address
        if (matchStreetAddress(arr, start, count)) {
            for (end  = count - 1; end > start + 2; end--) {
                // scroll from the end until we find a state
                if (matchStateStrong(arr, end, count)) {
                    for (int i = end - 2; i > start + 1; i--) {
                        // scroll in between looking for a street suffix
                        if (matchStreetSuffix(arr, i, count)) {
                            // if suffix is too far away then the address
                            // is invalid, so we continue searching
                            if (i - start + 1 > 7) break; 
                            // same if there is too large a gap between state and suffix
                            if (end - i + 1 > 6) break;

                            for (int j = i + 1; j < end; j++) {
                                // scroll from suffix to end looking for a city
                                if (matchCity(arr, j, end)) {
                                    // we now have something that matches a valid address
                                    clearStrArr(arr, count);
                                    free(arr);
                                    printf("state: %d, suffix: %d, city: %d\n", end, i, j);
                                    *start_index = strIndexFromArr(str, start);
                                    *end_index = strIndexFromArr(str, end);
                                    return 1;

                                }
                            }
                        }
                    }
                }
            }
        }
    }
    clearStrArr(arr, count);
    free(arr);
    return 0;
    
}
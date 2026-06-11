# Data Fields 

This is a listing of all types of data we need to store, with a standardized field name for each. By standardizing the names and formats of all information we are using, we will be able to easily pass information between modules. All strings should be stored in title case.

## Login
A database of login information, so that administrative actions may be securely accessed

username
 - A given user's username

encrypted_password
 - The hash value of a user's password

## Sports
A database of various adaptive sports and information about them.

sport_name
 - The specific type of sport (e.g. Para Ice Hockey, Nordic Skiing, Adaptive Dance). These names will be in title case.

sport_super_type
 - The more general type of sport, also in title case. For instance, both Beep Basketball and Wheelchair Basketball would share a sport_super_type Basketball

sport_category
 - A string with value "Indoor", "Outdoor", or "Water"

is_paralympic
 - A Boolean with value true iff the sport is featured in the paralympics

is_team_sport
 - A Boolean with value true iff the sport is played with a team

Is_equipment_intensive
 - A Boolean with value true iff the sport requires a lot of equipment. This is mostly up to developer discretion.

sport_description
 - A brief character buffer describing the sport.

sport_disability
 - The type of disability that this sport supports. If there are multiple, we will store this as multiple entries.


## Sports Instance
The core database of the project, listing individual sports opportunities. When users use the site, the results will be drawn from this database.

Note that it might be best to divide this into smaller databases? But I think in general we will want to access all of this information as a batch for a listing. If we are going to trim it down more we can always have our primary database have only information necessary for initials results display, and then pull from other databases if the user clicks on the listing. Either way, the following seem like good conventions to follow.

instance_id
 - A unique identifier for a given instance

instance_name
 - The name of the instance

sport_name
 - Name of the instance's sport, with the same domain of values as listed in the Sports database

instance_description
 - A brief description of the sport opportunity

instance_zip (String)
 - Zip code of the sport opportunity's location. This must be a string to allow for international postal codes

instance_city
 - City of the sport opportunity

instance_state
 - State of the sport opportunity

instance_address_line_1
 - First address line

instance_address_line_2

instance_country

instance_timezone
 - A string with the timezone used by the event host. For now, just use the format provided by the listing. If there is a reason to, we can standardize these later.

instance_start_time (int)
 - An integer representing the number of minutes past midnight at which the event begins. All times will be stored in their relative value at the instance's timezone. For instance, an event in EST at time 18:00 UTC will be stored as 14:00 (in minutes past midnight) 

instance_end_time (int)
 - An integer representing the number of minutes past midnight at which the event ends

instance_start_month (int)

instance_start_day (int)
 - The day of the month of the event. For example, April 28 has instance_day = 28

instance_start_year (int)
 - The year of the event. If a given instance does not appear to have a year listed, it should always be presumed to be the next possible instance of that date from the time of collection. 

instance_end_month (int)
instance_end_day (int)
instance_end_year (int)
 - It should be possible for someone to (for instance) host an overnight event, including on New Years' Eve

instance_allows_beginners
instance_allows_intermediate
instance_allows advanced
 - With the two above, a boolean value that indicates whether participants of a given skill level are allowed to attend.

organization_id
 - The unique ID for the organization hosting the event. NULL if no broader organization is hosting the event.

instance_repeat (string)
 - A value that indicates the frequency of the repetition.
	- None, Daily, Bidaily, Weekly, Biweekly (every other week), Monthly, Yearly, etc.

instance_age_floor
instance_age_ceil
 - With instance_age_floor, stores the age range of the instance. If both age_floor and age_ceil are null, then the instance is all-ages. If age_floor is null but age_ceil is not, then anyone up to the ceiling age may participate. If age_floor is not null, but age_ceil is, then anyone above the age floor may participate.

instance_cost (int)
 - The cost of this instance in specific. For instance, a $100 event would be stored as 10000

instance_quality (double)
 - An abstract score that conveys the relative reliability that an instance's information will be accurate to a real-life event. I've renamed this metric from "freshness" in the initial presentation, because I feel it's more appropriate, and allows us to use similar metrics for organizations without loss of meaning.

UTC_time (int)
 - A boolean variable with value 1 if the time stored in instance_start_time/instance_end_time is in UTC. Although we will be converting to local time when storing events in the database, this is a useful value to know so we can universalize times. Additionally, if we expand to store time information in UTC, we can still use this variable.

## Instance Sources

A small database storing what links were used as sources for a given instance.

instance_id

instance_site
 - A url that links to a webpage our data collector used as a source for the provided instance.

## Organizations

This database stores adaptive sports organizations. We can probably implement this later in development, as it's not strictly necessary, but having a reliable database of organizations that are likely to aggregate listings will likely speed up the update process by an enormous factor. Many field names in the markdown will not have descriptions, because their details are identical to those for sport instances.

org_id
 - A unique identifier for an organization

org_name
 - The full name of the organization

org_alias
 - A shorter version of the name, if it has one. This includes acronyms. Maybe unnecessary.

org_description
 - A short buffer storing a brief description of the organization

org_website
 - A url to the organization's main page

org_country

org_state

org_city

org_zip

org_address_line_1
 - All location data for an organization will be for the most relevant location to go. If you wanted to ask more about these sports opportunities in person, where would you go? The location data stored in this database should provide that information.

org_address_line_2

org_quality


## Grants

Grants are an important part of making an adaptive sport accessible. This database will captue information about grants so that the use can know if they are applicable to them

grant_name 
 - The name of the grant

grant_source
 - The source that is providing the grant. Typically the name of an organization

grant_type
 - Specifies what the grant pays for. "training," "equipment," "program," and "general" are provided by ASNM, but more may be added if they are relevant.

grant_amount_min
 - The minimum amount of the grant in cents. For instance, a $100 grant would be stored as 10000

grant_amount_max

grant_description
 - A short character buffer describing the grant

grant_eligibility_crit
 - Criteria for a grant to be eligible. This will also be a short character buffer, and will not be selected from a finite series of options, as grant eligibility criteria are too diverse for this

grant_application_url
 - A url that links to where someone may apply for the grant.

grant_source_url
 - A url that links to where this data was gathered from.

grant_is_open
 - true iff the grant is able to be applied to at the current moment.

grant_is_renewable
 - true iff the grant may be redeemed more than once over time.

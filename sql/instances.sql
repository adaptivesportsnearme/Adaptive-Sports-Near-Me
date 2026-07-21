-- Table: public.asnminstances

-- DROP TABLE IF EXISTS public."asnminstances";

CREATE TABLE IF NOT EXISTS public."asnminstances"
(
    instance_id character(10) COLLATE pg_catalog."default" NOT NULL,
    instance_name character(50) COLLATE pg_catalog."default",
    sport_name character(50) COLLATE pg_catalog."default",
    instance_desc character(200) COLLATE pg_catalog."default",
    instance_zip character(10) COLLATE pg_catalog."default",
    instance_city character(50) COLLATE pg_catalog."default",
    instance_state character(2) COLLATE pg_catalog."default",
    instance_address_line_1 character(100) COLLATE pg_catalog."default",
    instance_country character(2) COLLATE pg_catalog."default",
    instance_timezone character(32) COLLATE pg_catalog."default",
    instance_start_time integer,
    instance_end_time integer,
    instance_start_month integer,
    instance_start_day integer,
    instance_start_year integer,
    instance_end_month integer,
    instance_end_day integer,
    instance_end_year integer,
    instance_allows_beginners boolean,
    instance_allows_intermediate boolean,
    instance_allows_advanced boolean,
    organization_id character(10) COLLATE pg_catalog."default",
    instance_repeat character(32) COLLATE pg_catalog."default",
    instance_age_floor integer,
    instance_age_ceil integer,
    instance_cost double precision,
    instance_quality double precision,
    "UTC_time" integer,
    CONSTRAINT "ASNM-Instances_pkey" PRIMARY KEY (instance_id)
)

TABLESPACE pg_default;

ALTER TABLE IF EXISTS public."ASNM-Instances"
    OWNER to postgres;
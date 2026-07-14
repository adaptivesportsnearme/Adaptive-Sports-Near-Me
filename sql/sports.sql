-- Table: public.ASNM-Sports

-- DROP TABLE IF EXISTS public."ASNM-Sports";

CREATE TABLE IF NOT EXISTS public."ASNM-Sports"
(
    sport_name character(60) COLLATE pg_catalog."default" NOT NULL,
    sport_super_type character varying(60) COLLATE pg_catalog."default",
    sport_category character(60) COLLATE pg_catalog."default",
    is_paralympic boolean,
    is_team_sport boolean,
    is_equipment_intensive boolean,
    sport_desc character(200) COLLATE pg_catalog."default",
    sport_disability character(60) COLLATE pg_catalog."default",
    CONSTRAINT "ASNM-Program_pkey" PRIMARY KEY (sport_name)
)

TABLESPACE pg_default;

ALTER TABLE IF EXISTS public."ASNM-Sports"
    OWNER to postgres;
-- Table: public.ASNM-Grants

-- DROP TABLE IF EXISTS public."ASNM-Grants";

CREATE TABLE IF NOT EXISTS public."ASNM-Grants"
(
    grant_name character(50) COLLATE pg_catalog."default" NOT NULL,
    grant_source character(50) COLLATE pg_catalog."default",
    grant_type character(20) COLLATE pg_catalog."default",
    grant_amount_min integer,
    grant_amount_max integer,
    grant_desc character(200) COLLATE pg_catalog."default",
    grant_eligibility_crit character(200) COLLATE pg_catalog."default",
    grant_app_url character(200) COLLATE pg_catalog."default",
    grant_source_url character(200) COLLATE pg_catalog."default",
    grant_is_open boolean,
    grant_is_renewable boolean,
    CONSTRAINT "ASNM-Grants_pkey" PRIMARY KEY (grant_name)
)

TABLESPACE pg_default;

ALTER TABLE IF EXISTS public."ASNM-Grants"
    OWNER to postgres;
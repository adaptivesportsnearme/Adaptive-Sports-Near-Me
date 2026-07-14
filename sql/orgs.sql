-- Table: public.ASNM-Organizations

-- DROP TABLE IF EXISTS public."ASNM-Organizations";

CREATE TABLE IF NOT EXISTS public."ASNM-Organizations"
(
    org_id character(10) COLLATE pg_catalog."default" NOT NULL,
    org_name character(32) COLLATE pg_catalog."default",
    org_alias character(10) COLLATE pg_catalog."default",
    org_description character(200) COLLATE pg_catalog."default",
    org_website character(200) COLLATE pg_catalog."default",
    org_country character(6) COLLATE pg_catalog."default",
    org_state character(2) COLLATE pg_catalog."default",
    org_city character(50) COLLATE pg_catalog."default",
    org_zip character(10) COLLATE pg_catalog."default",
    org_address_line_1 character(200) COLLATE pg_catalog."default",
    org_quality double precision,
    CONSTRAINT "ASNM-Organizations_pkey" PRIMARY KEY (org_id)
)

TABLESPACE pg_default;

ALTER TABLE IF EXISTS public."ASNM-Organizations"
    OWNER to postgres;
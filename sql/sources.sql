-- Table: public.ASNM-Sources

-- DROP TABLE IF EXISTS public."ASNM-Sources";

CREATE TABLE IF NOT EXISTS public."ASNM-Sources"
(
    instance_id character(10) COLLATE pg_catalog."default",
    " instance_site" character(200) COLLATE pg_catalog."default"
)

TABLESPACE pg_default;

ALTER TABLE IF EXISTS public."ASNM-Sources"
    OWNER to postgres;
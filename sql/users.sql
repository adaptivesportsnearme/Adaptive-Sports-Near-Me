-- Table: public.ASNM-user

-- DROP TABLE IF EXISTS public."ASNM-user";

CREATE TABLE IF NOT EXISTS public."ASNM-user"
(
    "user" character(20) COLLATE pg_catalog."default" NOT NULL,
    pswd character(25) COLLATE pg_catalog."default",
    CONSTRAINT "ASNM-user_pkey" PRIMARY KEY ("user")
)

TABLESPACE pg_default;

ALTER TABLE IF EXISTS public."ASNM-user"
    OWNER to postgres;
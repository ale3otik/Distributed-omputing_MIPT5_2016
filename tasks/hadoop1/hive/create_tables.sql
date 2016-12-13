add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
add jar /opt/cloudera/parcels/CDH/jars/hive-serde-1.1.0-cdh5.7.0.jar;

USE BolshoyMorskoyPovar;

DROP TABLE IF EXISTS user_logs;
DROP TABLE IF EXISTS ip_data;
DROP TABLE IF EXISTS user_data;

CREATE EXTERNAL TABLE user_logs(
    ip STRING,
    date INT,
    url STRING,
    pagesize SMALLINT,
    statuscode SMALLINT,
    browser STRING
)
ROW FORMAT SERDE 'org.apache.hadoop.hive.serde2.RegexSerDe'
WITH SERDEPROPERTIES(
    "input.regex" = '^(\\S*)\\t{3}(\\d{8})\\S*\\t(\\S*)\\t(\\d{1,})\\t(\\d{1,})\\t(\\S*).*$',
    "output.format.string" = "%1$s %2$s %3$s"
)
STORED AS TEXTFILE
LOCATION '/data/user_logs/user_logs';


CREATE EXTERNAL TABLE ip_data(
    ip STRING,
    region STRING
)
ROW FORMAT SERDE 'org.apache.hadoop.hive.contrib.serde2.RegexSerDe'
WITH SERDEPROPERTIES(
    "input.regex" = '^(\\S*)\\t(\\S*).*$',
    "output.format.string" = "%1$s %2$s"
)
STORED AS TEXTFILE
LOCATION '/data/user_logs/ip_data';

CREATE EXTERNAL TABLE user_data(
    ip STRING,
    browser STRING,
    gender STRING,
    age SMALLINT
)
ROW FORMAT SERDE 'org.apache.hadoop.hive.serde2.RegexSerDe'
WITH SERDEPROPERTIES(
    "input.regex" = '^(\\S*)\\t(\\S*)\\t(\\S*)\\t(\\d{1,}).*$',
    "output.format.string" = "%1$s %2$s %3$s"
)
STORED AS TEXTFILE
LOCATION '/data/user_logs/user_data';


SELECT * FROM user_logs LIMIT 5;
SELECT * FROM ip_data LIMIT 5;
SELECT * FROM user_data LIMIT 5;
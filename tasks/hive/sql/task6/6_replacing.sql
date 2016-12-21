add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.9.0.jar;
add file http_to_ftp.sh;
USE BolshoyMorskoyPovar;

SELECT TRANSFORM(ip, date, url, pagesize, statuscode, browser) 
USING "http_to_ftp.sh" 
-- // your script
-- AS TRANSFORM -- ... // to be continued
FROM user_logs

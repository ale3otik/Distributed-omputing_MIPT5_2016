add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
USE BolshoyMorskoyPovar;

SELECT TRANSFORM(ip, date, url, pagesize, statuscode, browser) 
USING 'http_to_ftp.sh' 
-- // your script
-- AS ... // to be continued

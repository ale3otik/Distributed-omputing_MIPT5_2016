add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.9.0.jar;
USE BolshoyMorskoyPovar;

add jar jar/Mbyte.jar;
create temporary function mbyte as 'com.lab4udf.MbyteUDF';

SELECT ip as ip, date as date, url as url, pagesize as pagesize,  mbyte(pagesize) as mbytes , statuscode, browser FROM user_logs LIMIT 100;

add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
USE BolshoyMorskoyPovar;

add jar Mbyte/jar/Mbyte.jar;
create temporary function mbyte as 'com.lab4udf.MbyteUDF';

SELECT ip, date, url, mbyte(pagesize), statuscode, browser FROM user_logs LIMIT 10;

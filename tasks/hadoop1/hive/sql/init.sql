add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.9.0.jar;

drop database if exists BolshoyMorskoyPovar cascade;
create database BolshoyMorskoyPovar location '/user/s49705/metastore';

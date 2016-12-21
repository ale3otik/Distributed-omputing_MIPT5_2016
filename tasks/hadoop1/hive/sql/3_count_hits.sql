-- add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;
-- add jar /opt/cloudera/parcels/CDH/jars/hive-serde-1.1.0-cdh5.7.0.jar;

USE BolshoyMorskoyPovar;

WITH age_hits AS (
    SELECT u.age as age , count(u.age) as qnt
    FROM user_data u INNER JOIN user_logs l ON u.ip = l.ip
    GROUP by u.age
),
avg_hits AS(
    SELECT avg(qnt) as avg_qnt from age_hits
)

SELECT age_hits.age, age_hits.qnt 
FROM age_hits FULL JOIN avg_hits
WHERE age_hits.qnt < avg_hits.avg_qnt

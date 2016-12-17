USE BolshoyMorskoyPovar;

FROM user_logs SELECT date, count(DISTINCT statuscode) as qnt
GROUP by date
ORDER by qnt;

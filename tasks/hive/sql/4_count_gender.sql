USE BolshoyMorskoyPovar;

SELECT user_logs.browser, user_data.gender, count(user_data.gender) as qnt 
FROM user_data INNER JOIN user_logs  ON user_data.ip = user_logs.ip
GROUP BY user_logs.browser, user_data.gender
ORDER BY user_logs.browser

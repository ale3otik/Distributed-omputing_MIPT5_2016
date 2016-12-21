package com.lab4udf;

import org.apache.hadoop.hive.ql.exec.Description;
import org.apache.hadoop.hive.ql.exec.UDF;
import java.util.*;

@Description(
        name = "MbyteUDF",
        value = "Returns megabyte's representation",
        extended = "Example:\n" +
                "  SELECT mbyte(\'Hadoop\') FROM dummy;\n" +
                "Output: " +
                "  poodaH"
)
public class MbyteUDF extends UDF {
    /**
     * Dividing by 1024
     * @param str input string
     * @return value/1024 as string
     */
    public String evaluate(String str){ //The method MUST be called quite so!
        int value = Integer.parseInt(str);
        value = value/1024;
        return Integer.toString(value);
    }

    //I can implement few evaluate-methods with
    //different signatures. Hive will chose one of them
    //according to parameters in query.
}

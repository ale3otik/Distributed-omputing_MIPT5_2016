package ru.mipt.examples;

import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.hadoop.util.Tool;

import java.io.IOException;
import java.util.*;


/**
 * Created by instructor on 06.11.16.
 */
public class WordCount extends Configured implements Tool{

    /**
     * Маппер. На вход получаем сплит (фрагмент файла размером с HDFS-блок).
     * На выходе - множество пар (слово, 1).
     */
    public static class WordMapper extends Mapper<LongWritable, Text, Text, Text> {
        //здесь static не пишем т.к. значение переменной будет менятся в кажом маппере, а мапперы работаю параллельно
        private Text name = new Text();
        private Text word = new Text();

        /**
         * Мап-функция. На вход подаётся строка данных, на выходе - множество пар (слово, 1).
         * (Чтобы разбивка шла не по строкам, нужно изменть разделитель в конфигурации textinputformat.record.delimiter)
         * @param offset номер строки, начиная от начала входного сплита (не будет использован ни в этом примере, ни в ДЗ).
         * @param line строка текста.
         * @param context объект, отвечающий за сохранение результата.
         */
        public void map(LongWritable offset, Text line, Context context) throws IOException, InterruptedException {
            String [] splittedWords = line.toString().split("\\s+");
            // не реплика
            String characterName = splittedWords[0].replaceAll("^[^a-z,^A-Z,^а-я,^А-Я,^0-9]$", "");
            if(characterName.length() == 0 || characterName.equals("ПРИМЕЧАНИЯ")) {
                return;
            }

            name.set(characterName);
            for(int i = 1; i < splittedWords.length; ++i){
                String element = splittedWords[i];
                element = element.replaceAll("^[^a-z,^A-Z,^а-я,^А-Я,^0-9]$", "");
                if(element.length() == 0) {
                    continue;
                }
                word.set(element);
                context.write(name, word);
            }
            if(splittedWords.length > 1) {
                word.set("\n"); // сохраним уникальным символом перевода строки реплику
                context.write(name,word);
            }
        }
    }

    /**
     * Редьюсер. Суммирует пары (слово, 1) по ключу (слово).
     * На выходе получаем пары (уникальн_слово, кол-во).
     * В поставке Hadoop уже есть простейшие predefined reducers. Функционал данного редьюсера реализован в IntSumReducer.
     */
    public static class CountReducer extends Reducer<Text, Text, Text, Text>{
        //Пустой IntWritable-объект для экономии памяти, чтоб не создавать его при каждом выполнении reduce-функции
        private Text resultText = new Text();

        public void reduce(Text name, Iterable<Text> values, Context context) throws IOException, InterruptedException {
            HashSet<String> dictionary = new HashSet<String> ();
            Iterator<Text> it = values.iterator();
            int replica = 0;
            while (it.hasNext()){
                String word = it.next().toString();
                if(word.equals("\n")) {
                    ++replica;
                } else {
                    dictionary.add(word);
                }
            }
            String result = Integer.toString(dictionary.size())
            + " [" + Integer.toString(replica) + "]";
            resultText.set(result);
            context.write(name, resultText);
        }
    }

    @Override
    public int run(String[] strings) throws Exception {
        Path outputPath = new Path(strings[1]);

        // настройка Job'ы
        Job job1 = Job.getInstance();
        job1.setJarByClass(WordCount.class);

        job1.setMapperClass(WordMapper.class);
        job1.setReducerClass(CountReducer.class);

        job1.setOutputKeyClass(Text.class);
        job1.setOutputValueClass(Text.class);

        job1.setInputFormatClass(TextInputFormat.class);
        job1.setOutputFormatClass(TextOutputFormat.class);

        job1.setMapOutputKeyClass(Text.class);
        job1.setMapOutputValueClass(Text.class);

        job1.setNumReduceTasks(8); // по умолчанию задаётся 1 reducer

        TextInputFormat.addInputPath(job1, new Path(strings[0]));
        TextOutputFormat.setOutputPath(job1, outputPath);

        return job1.waitForCompletion(true)? 0: 1; //ждём пока закончится Job и возвращаем результат
    }

    public static void main(String[] args) throws Exception {
        new WordCount().run(args);
    }
}

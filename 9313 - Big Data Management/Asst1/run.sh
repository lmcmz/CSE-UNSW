unzip testdoc.zip
unzip InvertedIndex.zip

mkdir -p temp

hadoop com.sun.tools.javac.Main *.java -d temp

jar cf proj1.jar -C temp ./

rm -rf temp

hdfs dfs -rm -r output

hdfs dfs -rm input/*

hdfs dfs -put testdoc input

hadoop jar proj1.jar comp9313.proj1.Project1 input output 1

hdfs dfs -get output/p* ./


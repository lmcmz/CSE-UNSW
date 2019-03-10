package comp9313.proj1;

import java.io.IOException;
import java.util.HashMap;
import java.util.StringTokenizer;
import java.util.ArrayList;

import org.apache.hadoop.mapreduce.Partitioner;

import java.io.DataInput;
import java.io.DataOutput;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.*;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

public class Project1
{

	public static class TermIDPair implements WritableComparable<TermIDPair>
	{
		public String term;
		public Integer docID;

		public TermIDPair() {}

		public TermIDPair(String num_1, int num_2) {
			set(num_1, num_2);
		}

		public void set(String num1, int num2) {
			term = num1;
			docID = num2;
		}

		public int getdocID(){
			return docID;
		}

		public String getterm(){
			return term;
		}

		@Override
		public void readFields(DataInput in) throws IOException
		{
			docID=in.readInt();
			term=in.readUTF();
		}
		@Override
		public void write(DataOutput out) throws IOException
		{
			out.writeInt(docID);
			out.writeUTF(term);
		}

		@Override
		public int compareTo(TermIDPair kp) {
			int value = this.term.compareTo(kp.term);
			if (value != 0) {
				return value;
			}
			value = this.docID.compareTo(kp.docID);
			return value;
		}
		@Override
		public int hashCode(){
			return term.hashCode();
		}
	}

	public static class IntPair implements Writable
	{
		public Integer  docID, tf, df, docNum;

		public IntPair() {}

		public IntPair( int num_1, int num_2, int num_3, int num_4){
			set(num_1, num_2, num_3, num_4);
		}

		public void set(int num1, int num2, int num3, int num4){
			docID = num1;
			tf = num2;
			df = num3;
			docNum = num4;
		}

		public int getdocID(){
			return docID;
		}

		public int gettf(){
			return tf;
		}

		public int getdf()
		{
			return df;
		}

		public  int getdocNum() {return docNum;};

		@Override
		public void readFields(DataInput in) throws IOException
		{
			docID=in.readInt();
			tf=in.readInt();
			df=in.readInt();
			docNum=in.readInt();
		}
		@Override
		public void write(DataOutput out) throws IOException
		{
			out.writeInt(docID);
			out.writeInt(tf);
			out.writeInt(df);
			out.writeInt(docNum);
		}
	}

	public static class TokenizerMapper extends Mapper<Object, Text, TermIDPair, IntPair>
	{
		public void map(Object key, Text value, Context context) throws IOException, InterruptedException
		{
			String str = value.toString().toLowerCase();
			StringTokenizer itr = new StringTokenizer(str);
			String id_str = itr.nextToken();
//            System.out.println("DOC: "+ Integer.toString(docNum));
//            docNum = docNum + 1;
			while (itr.hasMoreTokens()){
				String term = itr.nextToken();
//                System.out.println("DOC: "+ term +"  "+id_str);
				IntPair pair = new IntPair( Integer.parseInt(id_str), 1, 1, 0);
				TermIDPair keyPair = new TermIDPair(term, Integer.parseInt(id_str));
				context.write(keyPair, pair);
			}

			Configuration conf = context.getConfiguration();
			int docNum = conf.getInt("docNum", 0);
			docNum += 1;
			conf.setInt("docNum", docNum);
//            System.out.println("NUMMMMMMMM: "+ String.valueOf(docNum));
		}
	}

	public static class IntSumCombinerReducer extends Reducer<TermIDPair, IntPair, TermIDPair, IntPair>
	{
		public void reduce(TermIDPair key, Iterable<IntPair> values, Context context) throws IOException, InterruptedException
		{
			Configuration conf = context.getConfiguration();
			int docNum = conf.getInt("docNum", 0);
//            System.out.println("UUUUUUUUUUUUUUUUU1: "+ String.valueOf(docNum));

			int sumDF = 0;
			ArrayList<Integer> set = new ArrayList<Integer>();
			ArrayList<Integer> idArray = new ArrayList<Integer>();
			System.out.println("KEY: "+key.term+"    "+String.valueOf(key.docID));
			for (IntPair val : values) {
				int docID = val.docID;
				idArray.add(docID);

				if(!set.contains(docID)) {
					set.add(docID);
					sumDF += 1;
				}
			}

			for(int i=0; i< set.size(); i++) {
				int sumTF = 0;
				for (int j=0; j<idArray.size(); j++) {
					if( set.get(i).equals(idArray.get(j))) {
						sumTF += 1;
					}
				}
//                System.out.println("DOC_2: "+ key.term +"   "+ String.valueOf(set.get(i))+"  "+String.valueOf(sumTF)+"   "+String.valueOf(sumDF));
				context.write(key, new IntPair(set.get(i),sumTF,sumDF, docNum));
			}
		}
	}

	public static class IntSumReducer extends Reducer<TermIDPair, IntPair, Text, Text>
	{
		public void reduce(TermIDPair key, Iterable<IntPair> values, Context context) throws IOException, InterruptedException
		{
			for (IntPair val : values) {
				double df = (double) val.docNum/val.df;
				double tfidf = Math.log10(df) * val.tf;
				String result = String.valueOf(val.docID) +","+ String.valueOf(tfidf);
//                System.out.println("DOC_3: "+ key.term +"  "+ String.valueOf(val.docID)+"  "+String.valueOf(val.tf)+"   "+String.valueOf(val.df) +"  "+ String.valueOf(tfidf));
				Text out_key = new Text(key.term);
				context.write(out_key, new Text(result));
			}
		}
	}

	public static class FirstPartitioner extends Partitioner<TermIDPair, IntPair>
	{
		@Override
		public int getPartition(TermIDPair key, IntPair value, int numberOfPartitions) {
//            System.out.println("KEY_100: "+ key.term +"   "+ numberofreducer);
			return Math.abs(key.term.hashCode()) % numberOfPartitions;
		}
	}

	public static class KeyComparator extends WritableComparator {
		protected KeyComparator() {
			super(TermIDPair.class, true);
		}
		@Override
		public int compare(WritableComparable w1, WritableComparable w2) {
			TermIDPair ip1 = (TermIDPair) w1;
			TermIDPair ip2 = (TermIDPair) w2;
			return ip1.term.compareTo(ip2.term);
		}
	}

	public static void main(String[] args) throws Exception
	{
		Configuration conf = new Configuration();
		conf.setInt("docNum", 0);

		Job job = Job.getInstance(conf, "word count");

		job.setPartitionerClass(FirstPartitioner.class);
		job.setNumReduceTasks(Integer.parseInt(args[2]));
		job.setJarByClass(Project1.class);

		job.setCombinerKeyGroupingComparatorClass(KeyComparator.class);
		job.setMapperClass(TokenizerMapper.class);
		job.setCombinerClass(IntSumCombinerReducer.class);
		job.setReducerClass(IntSumReducer.class);

		job.setMapOutputKeyClass(TermIDPair.class);
		job.setMapOutputValueClass(IntPair.class);

		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(Text.class);
		FileInputFormat.addInputPath(job, new Path(args[0]));
		FileOutputFormat.setOutputPath(job, new Path(args[1]));
		System.exit(job.waitForCompletion(true) ? 0 : 1);
	}
}





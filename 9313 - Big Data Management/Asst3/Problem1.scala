//package comp9313.proj2
import org.apache.spark.SparkContext
import org.apache.spark.SparkContext._
import org.apache.spark.SparkConf
import scala.util.matching.Regex


object Problem1 {
	def main(args: Array[String]) {
		val conf = new SparkConf().setAppName("Problem1").setMaster("local")
		val sc = new SparkContext(conf)
		val inputPath = args(0)
		val outputPath = args(1)
		val textFile = sc.textFile(inputPath)
		val regx = new Regex("^[a-z]+.*$")

		val lines = textFile.map(_.toLowerCase.split("\\n+"))
		val tokens = lines.map(_.map(words => words.split("[\\s*$&#/\"'\\,.:;?!\\[\\](){}<>~\\-_]+")))
		val r_token = tokens.map(_.flatMap(word => word.filter(w => regx.pattern.matcher(w).matches())))

		val mapRed_1 = r_token.map(_.zipWithIndex).flatMap(words =>
			for{
				(word1, i) <- words
				(word2, j) <- words
				if i < j
			} yield ((word1,word2), 1)).reduceByKey((x,y)=>(x+y))

		mapRed_1.collect.foreach(println)

		val mapRed_2 = r_token.map(_.zipWithIndex).flatMap(words =>
			for{
				(word1, i) <- words
				(word2, j) <- words
				if i < j
			} yield (word1, 1)).reduceByKey((x,y)=>(x+y))

		mapRed_2.collect.foreach(println)

		val result = for {
			((w1,w2),count) <- mapRed_1.collect
			(word, sum) <- mapRed_2.collect
			if w1 == word
		} yield (w1, w2, count.toDouble/sum)

		val final_output = result.sortBy(r => (r._1, r._3, r._2))( Ordering.Tuple3(Ordering.String, Ordering.Double.reverse, Ordering.String))
		sc.parallelize(final_output).map(r => f"${r._1} ${r._2} ${r._3}").saveAsTextFile(outputPath)
	}
}
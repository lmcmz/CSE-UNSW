package comp9313.proj3

import org.apache.spark.SparkContext
import org.apache.spark.SparkContext._
import org.apache.spark.SparkConf
import scala.math.BigDecimal
import scala.collection.mutable.HashSet
import Math.ceil
import Math.min
import Math.max

object SetSimJoin {
	def main(args: Array[String]) {

		val conf = new SparkConf().setAppName("proj3")
//		.setMaster("local")
		val sc = new SparkContext(conf)

		val file_1 = args(0)
		val file_2 = args(1)
		val out_file = args(2)
		val t = args(3).toDouble	// Threshold

		
		//Mapping File_1		
		var mapRed_1 = sc.textFile(file_1).flatMap { l =>
			var list = l.split(" ")
			var id = list(0).toInt
			val s_list = list.drop(1).sorted
			var prefix = ceil((s_list.length).toDouble * (1 - t)).toInt + 1
			if (prefix > s_list.length) {
				prefix = s_list.length
			}
			for {
				i <- 0 until prefix
			} yield (s_list(i), ( id, 'f', s_list))
		}

		//Mapping File_2
		var mapRed_2 = sc.textFile(file_2).flatMap { l =>
			var list = l.split(" ")
			var id = list(0).toInt
			val s_list = list.drop(1).sorted
			var prefix = ceil((s_list.length).toDouble * (1 - t)).toInt + 1
			if (prefix > s_list.length) {
				prefix = s_list.length
			}
			for {
				i <- 0 until prefix
			} yield (s_list(i), ( id, 'l', s_list))
		}

		//Groupbykey
		var grouped = mapRed_1.union(mapRed_2).groupByKey()

		//    grouped.collect.foreach(println)
		//    var filtered = grouped.filter { case (id, arr) => (arr.toList.length >= 2) }
		//    filtered.collect.foreach(println

		//Generate candidate pair
		var pairs = grouped.flatMap { case (id, arr) =>
			for {
				(eid_1, flag_1, earr_1) <- arr;
				(eid_2, flag_2, earr_2) <- arr;
				if flag_1 != flag_2
				if (flag_1 == 'f')
				if (eid_1 != eid_2)
				if ((min(earr_1.length, earr_2.length).toDouble / max(earr_1.length, earr_2.length).toDouble) >= t)
			} yield ((eid_1, eid_2), (earr_1, earr_2))
		}

		// Calculate simlarity and filter the result
		val fsim = pairs.map{ case((eid_1, eid_2), (earr_1, earr_2)) => {
			var union = earr_1.union(earr_2).distinct.length
			var injunct = earr_1.intersect(earr_2).length
			var sim = BigDecimal(injunct.toDouble / union.toDouble).setScale(6, BigDecimal.RoundingMode.HALF_UP).toDouble
			( eid_1, eid_2, sim)
		}}.filter{ x => x._3 >= t}.distinct.sortBy(t => (t._1, t._2, t._3)).map(x => f"(${x._1},${x._2})\t${x._3}")

		// Output result
		fsim.saveAsTextFile(out_file)
		sc.stop()
	}
}
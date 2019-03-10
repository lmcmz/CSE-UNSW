package comp9313.proj2

import org.apache.spark.SparkContext
import org.apache.spark.SparkContext._
import org.apache.spark.SparkConf
import org.apache.spark.graphx._
import org.apache.spark.rdd.RDD
import scala.collection.mutable.Set

object Problem2 {
	def main(args: Array[String]) {

		val conf = new SparkConf().setAppName("Problem2").setMaster("local")
		val sc = new SparkContext(conf)

		val fileName = args(0)

		val k = args(1).toInt
		val rawData = sc.textFile(fileName)

		// Create an RDD for edges
		val relationships: RDD[Edge[Boolean]] = rawData.map(x => x.split(" ")).map(x => Edge(x(1).toLong, x(2).toLong, true))

		// Create an RDD for the vertices
		val test_1: RDD[VertexId] = rawData.map(x => x.split(" ")).map(x => x(1).toLong)
		val test_2: RDD[VertexId] = rawData.map(x => x.split(" ")).map(x => x(2).toLong)
		var vSet = Set[VertexId]()
		test_1.collect.foreach(x => vSet += x)
		test_2.collect.foreach(x => vSet += x)
		//    println(vSet)
		var array: Array[(VertexId, Set[(VertexId, Set[VertexId])])] = vSet.map(x => (x, Set((x, Set(x))))).toArray
		var vertices: RDD[(VertexId, Set[(VertexId, Set[VertexId])])] = sc.parallelize(array)

		// Create the graph
		var graph = Graph(vertices, relationships)

		// Check the graph
		//    graph.triplets.collect().foreach(println)

		val initialMsg = Set[(VertexId, Set[VertexId])]()

		def vprog(vertexId: VertexId, value: Set[(VertexId, Set[VertexId])], message: Set[(VertexId, Set[VertexId])]): Set[(VertexId, Set[VertexId])] = {
			if (message.equals(initialMsg)) {
				value
			} else {
				value ++ message
			}
		}

		def sendMsg(triplet: EdgeTriplet[Set[(VertexId, Set[VertexId])], Boolean]): Iterator[(VertexId, Set[(VertexId, Set[VertexId])])] = {
			val sourceVertex = triplet.srcAttr
			val result = sourceVertex.map(set => (set._1, set._2 + triplet.srcId))
			Iterator((triplet.dstId, result))
		}

		def mergeMsg(msg1: Set[(VertexId, Set[VertexId])], msg2: Set[(VertexId, Set[VertexId])]): Set[(VertexId, Set[VertexId])] = {
			msg1 ++ msg2
		}

		val minGraph = graph.pregel(initialMsg,
			k,
			EdgeDirection.Out)(
			vprog,
			sendMsg,
			mergeMsg)

		//    minGraph.vertices.collect.foreach { case (vid, set1) => println(set1) }

		//    println("---------------------------------")
		var tempSet = Set[(VertexId, Set[VertexId])]()
		minGraph.vertices.collect.foreach { case (vid, set1) => set1.map { case x =>
			if (x._2.size == k) {
				tempSet += x
			}
		}
		}
		//    println(tempSet)
		var finalSet = Set[(Int, Set[VertexId])]()

		for ((id_1, set_1) <- tempSet) {
			var count = 0
			for ((id_2, set_2) <- tempSet) {
				if (set_1 == set_2) {
					count += 1
				}
			}
			finalSet.add((count, set_1))
		}

		for ((count, set) <- finalSet) {
			if (count != set.size) {
				finalSet.remove((count, set))
			}
		}
		//    println(finalSet)
		println(finalSet.size)
	}
}
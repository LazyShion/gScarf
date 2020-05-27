## Scaling Fine-grained Modularity Clustering for Massive Graphs
The implementation of gScarf proposed in IJCAI'19.
If you use our software, **please cite the following paper**.

```
Hiroaki Shiokawa, Toshiyuki Amagasa, Hiroyuki Kitagawa, 
"Scaling Fine-grained Modularity Clustering for Massive Graphs,"
In Proceedings of the 28th International Joint Conference on Artificial Intelligence (IJCAI2019), pp.4597-4604, Macao, China, August 2019. 
```
``` 
@inproceedings{ShiokawaTK2019,
  title     = {{Scaling Fine-grained Modularity Clustering for Massive Graphs}},
  author    = {Shiokawa, Hiroaki and Amagasa, Toshiyuki and Kitagawa, Hiroyuki},
  booktitle = {Proceedings of the 28th International Joint Conference on
               Artificial Intelligence, {IJCAI-19}},
  publisher = {International Joint Conferences on Artificial Intelligence Organization},             
  pages     = {4597--4604},
  year      = {2019},
  month     = {7},
  doi       = {10.24963/ijcai.2019/639},
  url       = {https://doi.org/10.24963/ijcai.2019/639},
}
```

### LICENSE
This software is released under the MIT license. Please read `LICENSE.txt` for details.


## How to Use
### Requirements
gScarf requires the following softwares.
* gcc Version 4.8.5 (or later)
* boost C++ Libraries Version 1.53.0 (or later)

We have confirmed that our software works on the following environments.
* CentOS 7.6
* MacOS 10.14.6

### Build
1. Install *gcc* and *boost libraries* on your environment.
2. Rewrite `Makefile`. You should add your file path of "libboost_serialization.a" in the directory of the boost libraries to `Makefile` like as follows:
```Makefile
LIB= (add your path of "libboost_serialization.a" here)
```
3. Build the software.
```
$ make clean; make
```
4. If you can find `gscarf` and `convert`, the build has been successfully completed.

### Usage
#### Input file
Input file must be formatted as a list of edges included in a given graph; each line represents a pair of node IDs that are connected via an edge. The nodes in the same line must be spanned by `TAB` or `space` like as follows:
``` sample_graph.txt
1	90
1	109
1	207
1	282
1	321
1	699
...
```

#### File conversion
`gscarf` reads the given graph by the CRS format, and this requires a file conversion process. 
To covert the input file (`sample_graph.txt`) into the CRS format (`sample_graph.bin`), you should run `convert` like as follows:
``` convert
$ ./convert -i sample_graph.txt -o sample_graph.bin
```
`convert` requires two options, `-i` and `-o`, that specify names of the input file and the CRS formatted file, respectively.

Note that `convert` will converts the input file as an undirected graph in this file conversion process even though the input file represents a directed graph.

#### Clustering
Finally, we can run the clustering by `gscarf` like as follows:
```
$ ./gscarf sample_graph.bin
```
`gscarf` has the following two options.
|Option|Description|
|------|:----------|
|`-v`  |Display a clustering result of gScarf. This result is formatted as a list of node ID and its corresponding cluster ID.|
|`-r`  |Display statistics (e.g., # of clusters, avg. size of clusters, etc.)|

##### Examples
* Display the clustering result; each line indicates a pair of node ID and corresponding cluster ID.
For example, in the following example, nodes `991` and `993` are included in a cluster `21`.
```
$ ./gscarf -v sample_graph.bin
...
991	21
992	8
993	21
994	8
995	4
996	4
997	8
998	17
999	4
1000	4
```

* You can display statistics of the clustering result by using option `-d`.
```
$ ./gscarf sample_graph.bin -d
Number of nodes: 1001
Number of edges: 15534
Runtime for clustering (sec.):0
Modularity Q:0.851665
Number of clusters:36
Average cluster size:27.8056
Largest cluster size:49
```

## Reference
* Hiroaki Shiokawa, Toshiyuki Amagasa, Hiroyuki Kitagawa, **"Scaling Fine-grained Modularity Clustering for Massive Graphs,"**  In _Proceedings of the 28th International Joint Conference on Artificial Intelligence (IJCAI2019)_, pp.4597-4604, Macao, China, August 2019. 
* http://www.kde.cs.tsukuba.ac.jp/~shion/index.html.en

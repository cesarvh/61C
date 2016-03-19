from simple_page_rank import SimplePageRank

"""
This class implements the pagerank algorithm with
backwards edges as described in the second part of 
the project.
"""
class BackedgesPageRank(SimplePageRank):

    """
    The implementation of __init__ and compute_pagerank should 
    still be the same as SimplePageRank.
    You are free to override them if you so desire, but the signatures
    must remain the same.
    """
    def __init__(self, input_rdd):
        self.input_rdd = input_rdd

    def compute_pagerank(self, num_iters):
        nodes = self.initialize_nodes(self.input_rdd)
        num_nodes = nodes.count()
        for i in range(0, num_iters):
            nodes = self.update_weights(nodes, num_nodes)
        return self.format_output(nodes)
    """
    This time you will be responsible for implementing the initialization
    as well. 
    Think about what additional information your data structure needs 
    compared to the old case to compute weight transfers from pressing
    the 'back' button.
    """
    @staticmethod
    def initialize_nodes(input_rdd):
        # YOUR CODE HERE
        # The pattern that this solution uses is to keep track of 
        # (node, (weight, targets, old_weight)) for each iteration.
        # When calculating the score for the next iteration, you
        # know that 10% of the score you sent out from the previous
        # iteration will get sent back.
        def emit_edges(line):
            # ignore blank lines and comments
            if len(line) == 0 or line[0] == "#":
                return []
            # get the source and target labels
            source, target = tuple(map(int, line.split()))
            # emit the edge
            edge = (source, frozenset([target]))
            # also emit "empty" edges to catch nodes that do not have any other node 
            # leading into them, but we still want in our list of nodes
            self_source = (source, frozenset())
            self_target = (target, frozenset())
            return [edge, self_source, self_target]

        # collects all outgoing target nodes for a given source node
        def reduce_edges(e1, e2):
            return e1 | e2

        # sets the weight of every node to 0, and formats the output
        # to the specified format of (source (weight, targets))
        def initialize_weights((source, targets)):
            return (source, (1.0, targets, 1.0))

        nodes = input_rdd\
                .flatMap(emit_edges)\
                .reduceByKey(reduce_edges)\
                .map(initialize_weights)
        return nodes

    """
    You will also implement update_weights and format_output from scratch.
    You may find the distribute and collect pattern from SimplePageRank
    to be suitable, but you are free to do whatever you want as long
    as it results in the correct output.
    """
    @staticmethod
    def update_weights(nodes, num_nodes):
        # YOUR CODE HERE

        def distribute_weights((node, (weight, targets, old_weight))):
            keyvals = []
            if (len(targets) > 0):
                targetweight = (0.85 * weight) / len(targets)
                for target in targets:
                    keyvals.append((target, (targetweight, frozenset())))
            # 0.10 chance of going to previous node, weight given to prev node is 0.1 * prev node to node
            else:
                # for every other node, there is a 0.85 chance of going to it
                counter = num_nodes - 1
                dist_weight = weight * 0.85 / (num_nodes  - 1)
                # forgot to multilply the percentage by the weight!
                while (counter >= 0):
                # for other_node in nodes:
                    if counter != node:
                        keyvals.append((counter, (dist_weight, frozenset())))
                    counter -= 1
            keyvals.append((node, (0.10 * old_weight, targets, weight)))
                
            keyvals.append((node, (0.05 * weight, targets, weight)))
            return keyvals

            # return value is (node, (weight given to node from prev_node, targets, old_weight))
        def collect_weights((node, values)):
            #
            alltargets = frozenset()
            totalWeight = 0
            for value in values:
                weight = value[0]
                targets = value[1]
                if len(value) == 2:
                    totalWeight += weight
                    alltargets = alltargets | targets
                else:
                    old_weight = value[2]
                    totalWeight += weight
                    alltargets = alltargets | targets
                    prev_weight = old_weight

            return (node, (totalWeight, alltargets, prev_weight))

        return nodes\
                .flatMap(distribute_weights)\
                .groupByKey()\
                .map(collect_weights) 
                     
    @staticmethod
    def format_output(nodes): 
        # YOUR CODE HERE
        return nodes\
                .map(lambda (node,(weight, targets, old_weight)): (weight, node))\
                .sortByKey(ascending = False)\
                .map(lambda (weight, node): (node, weight))

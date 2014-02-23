# coding: utf-8

import collections


def print_graph(work_graph):
    for k, v in work_graph.items():
        print k,
        for edge in v:
            print edge,
        print

def main():
    source_graph = {1: [2, 4], 2: [1, 4, 3], 3: [2, 4], 4: [1, 2, 3]}

    class Edge(object):
        def __init__(self, begin, end):
            # Исходное пложение ребра
            self.source_ = (begin, end)

            # Рабочее пложение ребра
            self.work = (begin, end)

        def __str__(self):
            return str(self.work)

    # BAD! Но это указатели, а не копии объектов. Может не так все печально?
    work_graph = collections.defaultdict(list)
    for k, v in source_graph.items():
        for value in v:
            work_graph[k].append(Edge(k, value))



    # слить два узла
    print 'fusing'
    begin = 1
    end = 2
    begin_value = work_graph[begin]
    end_value = work_graph[end]
    del work_graph[end]  # в графе уже нет! ссылка локальна

    # Можно вливать узел

    print end_value

    print 'rest'
    print_graph(work_graph)

main()
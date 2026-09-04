"""
Bundeswettbewerb Informatik 2025 Runde 2 Aufgabe 2
Autor: Tao Zheng
"""

import numpy as np
import numpy.random as rnd
import math
import alns
from functools import cmp_to_key

# Berechnet Distanz gemäß Satz des Pythagoras
def calc_dist(a, b):
    return ((a[0] - b[0])**2 + (a[1] - b[1])**2)**0.5

# Lösung generieren
def solve(filename):
    # Daten einlesen
    # -> positions, nTrees, maxDist
    with open(f"A2_Gießroboter\\{filename}.txt", "r") as f:
        Input = f.read()
        lines = Input.strip().split("\n")
        maxDist = float(lines[0])
        nTrees = int(lines[1])
        positions = []
        pos_to_ids = []

        minx = 1e9
        maxx = 0
        miny = 1e9
        maxy = 0

        for line in lines[2:]:
            xy = line.split(" ")
            pos_to_ids.append(xy[0])

            minx = min(minx, int(xy[1]))
            maxx = max(maxx, int(xy[1]))
            miny = min(miny, int(xy[2]))
            maxy = max(maxy, int(xy[2]))

            positions.append((float(xy[1]), float(xy[2])))
        del Input, lines, line, xy
    
    # 2D-Bucket erstellen
    # Abstand abhängig von der Dichte berechnen
    density = nTrees / ((maxx - minx) * (maxy - miny)) * 100
    splitfactor = 100
    if density > 0.1:
        splitfactor = 49
    if density > 0.5:
        splitfactor = 23
    
    # 2D-Bucket generieren
    grid = np.frompyfunc(list, 0, 1)(np.empty((math.ceil((maxx - minx)/splitfactor) + 1, math.ceil((maxy - miny)/splitfactor) + 1), object))

    # Alle Bäume in die Buckets einspeichern und für alle Bäume ihre Bucket-Koordinate speichern
    idtogrid = []
    for i in range(nTrees):
        x, y = positions[i]
        gridx = math.floor((x - minx) / splitfactor)
        gridy = math.floor((y - miny) / splitfactor) 
        grid[gridx, gridy].append(i)
        idtogrid.append((gridx, gridy))

    # Hilfsfunktion: Alle Bäume aus der Nähe eines Baumes zurückgeben
    def getNearbyTrees(id, d):
        # id: Baumid
        # d: Maximale Buckets Distanz zu Bucket des Baums
        x, y = idtogrid[id]
        x1 = max(0, x - d)
        x2 = min(grid.shape[0], x + d + 1)
        y1 = max(0, y - d)
        y2 = min(grid.shape[1], y + d + 1)

        # Alle Bäume aus naheliegenden Buckets zurückgeben.
        return [a for b in grid[x1:x2, y1:y2].ravel() for a in b]
    
    # Für alle Bäume naheliegende Bäume einspeichern.
    nearbyList = []
    for i in range(nTrees):
        d = 2
        while True:
            # Für jeden Baum muss mindestens 10 oder alle nahliegenden Bäume eine Referenz gespeichert werden.
            nearby = getNearbyTrees(i, d)
            if len(nearby) < min(10, nTrees):
                d += 1
                continue
            nearby.remove(i)
            nearbyList.append(nearby)
            break
    
    # Hilfsfunktion: Distanz zwischen Knoten u und v speichern, wobei nur nötige Distanzen (abhängig von naheliegenden Knoten) gespeichert werden.
    storage = [{} for i in range(nTrees)]
    def dist(a, b):
        # Überprüfen ob die Distanz schon bereits berechnet wurde.
        value = storage[a].get(b, None)
        if value is None:
            # Falls es noch nicht berechnet wurde: Distanz berechnen und für zukünftige Verwendung speichern.
            value = calc_dist(positions[a], positions[b])
            storage[a][b] = value
            storage[b][a] = value
        return value

    # Initialisierung: Eine mögliche Lösung berechnen
    pre_positions = []
    for i in range(nTrees):
        x, y = positions[i]
        pre_positions.append((x, y, i))
    
    # Hilfsfunktion für die Sortierung
    def sortxy(a, b):
        return (a[0] + a[1]) - (b[0] + b[1]) 
    pre_positions = sorted(pre_positions, key=cmp_to_key(sortxy), reverse=True)

    # Überprüfen, ob ein Knoten schon bereits eine Route bekommen hat.
    pre_calculated = [0] * nTrees
    
    # Anfängliche Route sowie Zuweisungen initialisieren.
    init_Routes = []
    init_assignment = [None] * nTrees

    # Für alle Bäume mithilfe eines Greedy Algorithmus eine Route zuweisen.
    for i in range(nTrees):
        if pre_calculated[i]: continue
        pre_calculated[i] = 1

        pre_route = []
        pre_route.append(i)

        pre_length = 0
        # Failsafe: Falls es nur einen Baum existiert.
        while True:
            # Beste nächste Knoten berechnen.
            pre_bestDist = 1e9
            pre_bestId = -1
            for j in nearbyList[pre_route[-1]]:
                # Durch alle Nachbarn der zuletzt eingefügten Knoten durchgehen und die nächstgelegenen noch nicht eingefügten Knoten merken.
                if pre_calculated[j]: continue
                pre_Dist = dist(j, pre_route[-1])
                if pre_Dist < pre_bestDist:
                    pre_bestDist = pre_Dist
                    pre_bestId = j
            # Fall: Es gibt keine unbesuchten Nachbarn 
            if (pre_bestId == -1): break
            # Überprüfen, ob der Zeitlimit durch das Einfügen der besten Knoten weiterhin gehalten wird.
            if (pre_length + pre_bestDist + dist(pre_bestId, pre_route[0]) <= maxDist):
                pre_length += pre_bestDist
                pre_calculated[pre_bestId] = 1
                pre_route.append(pre_bestId)
            else:
                break
        # Route speichern und alle Bäume auf ihre Route zuweisen.
        init_Routes.append(pre_route)
        for j in pre_route:
            init_assignment[j] = init_Routes[-1]

    # Adaptive Large Neighbor Search initialisieren
    # Quelle: https://alns.readthedocs.io/en/latest/examples/capacitated_vehicle_routing_problem.html#Solution-state
    class VRP(alns.State):
        # Diese Klasse stellt einen möglichen Zustand der Routenaufteilung dar.
        def __init__(self, routes, unassigned = None, assignment = None):
            # Neuen Zustand erstellen und von anderen Zuständen trennen.
            self.routes = routes
            if unassigned is None:
                self.unassigned = []
            else:
                self.unassigned = unassigned
            if assignment is None:
                self.assignment = []
            else:
                self.assignment = assignment
        
        # Zielfunktion: Ziel -> minimieren
        def objective(self):
            nRoutes = sum(1 for i in self.routes if len(i) > 0)
            return len(self.unassigned) * maxDist * 1.5 + nRoutes * maxDist * 1.3 + sum(self.route_cost(route) for route in self.routes)
        
        @property
        def cost(self):
            return self.objective()

        # Länge einer Route berechnen
        @staticmethod
        def route_cost(route):
            cost = dist(route[0], route[-1])
            for i in range(len(route) - 1):
                cost += dist(route[i], route[i + 1])
            return cost
    
    # Maximale Zerstörungsgrad abhängig von der Anzahl der Bäume berechnen
    # Quelle: https://alns.readthedocs.io/en/latest/examples/capacitated_vehicle_routing_problem.html#Destroy-operators
    if nTrees <= 50:
        math_destruction = 0.2
    else:
        math_destruction = 0.2 - 0.0011 * (nTrees - 50) 
    degree_of_destruction = max(0.05, math_destruction)

    # Maximale Anzahl der zerstörten Bäume abhängig von der Anzahl der Bäume berechnen
    trees_to_remove = int((nTrees) * degree_of_destruction)

    # Zerstörungsfunktion: Mehrere zufällige Knoten aus ihrer Route rauswerfen.
    def multi_random_removal(state, rng):
        destroyed_Routes = [route.copy() for route in state.routes if len(route) > 0]
        assignment = [None] * nTrees
        for route in destroyed_Routes:
            for tree in route:
                assignment[tree] = route
        destroyed = VRP(destroyed_Routes, state.unassigned.copy(), assignment)
        
        for i in range(rng.randint(1, int(trees_to_remove) + 2)):
            if not destroyed.routes:
                break
            routeId = rng.randint(len(destroyed.routes))
            treeId = rng.randint(len(destroyed.routes[routeId]))
            destroyed.assignment[destroyed.routes[routeId][treeId]] = None
            destroyed.unassigned.append(destroyed.routes[routeId].pop(treeId))
            if len(destroyed.routes[routeId]) == 0:
                destroyed.routes.pop(routeId)
        return destroyed

    # Nicht zugewiesene/rausgeworfene Knoten einer Route zuweisen.
    def fast_repair(state, rng):
        rng.shuffle(state.unassigned)
        while len(state.unassigned) != 0:
            tree = state.unassigned.pop()
            inserted = False
            
            rng.shuffle(nearbyList[tree])
            for nearbyTree in nearbyList[tree]:
                route = state.assignment[nearbyTree]
                if route is None:
                    continue
                for i in range(len(route) + 1):
                    newRoute = route[:i] + [tree] + route[i:]
                    if VRP.route_cost(newRoute) <= maxDist:
                        route.insert(i, tree)
                        state.assignment[tree] = route
                        inserted = True
                        break
                if inserted: break
            else:
                # Falls keine Route gefunden werden konnte: Neue Route erstellen.
                state.routes.append([tree])
                state.assignment[tree] = state.routes[-1]
        return state
    
    # Anfangszustand mit der Anfangslösung erstellen
    state = VRP(init_Routes, assignment=init_assignment)

    # Einzelne Operatoren anwenden und speichern
    alns_obj = alns.ALNS(rnd.RandomState(42))
    alns_obj.add_destroy_operator(multi_random_removal)
    alns_obj.add_repair_operator(fast_repair)

    select = alns.select.RouletteWheel([3, 1, 1, 0.1], 0.80, 1, 1)
    accept = alns.accept.SimulatedAnnealing(0.8, 0.01, 0.85)
    stop = alns.stop.MaxRuntime(0.2)

    # Lösung berechnen
    result = alns_obj.iterate(state, select, accept, stop)
    best = result.best_state

    routes = [route for route in best.routes if route]
    print(f"Solution: {len(routes)} routes")
    
    # Failsafe Check: Alle Bäume sollen eine Route zugewiesen bekommen haben.
    counter = 0
    for route in routes:
        counter += len(route)
    assert counter == nTrees

    # Ergebnis in der Datei speichern.
    with open("Ausgabe\\" + filename + "_out.txt", "w") as f:
        f.write(f"{len(routes)}\n")
        f.write(f"{int(maxDist)}\n")

        for i in range(len(routes)):
            f.write(f"{int(VRP.route_cost(routes[i]))}\n") #DEBUG
            #f.write(f"{int(Lengths[i]**0.5)}\n") # floor
            posx, posy = positions[routes[i][0]]
            f.write(f"{int(posx)} {int(posy)}\n")
            for j in routes[i]:
                f.write(f"{pos_to_ids[j]} ")
            f.write("\n")


if __name__ == '__main__':
    from time import time
    filenames = [
        "roboter_Beispiel",
        "roboter01",
        "roboter02",
        "roboter03",
        "roboter04",
        "roboter05",
        "roboter06",
        "roboter07",
        "roboter08",
        "roboter09",
        "roboter10",
        "roboter11",
    ]
    for file in filenames:
        print(file)
        start = time()
        solve(file)
        print(f"Execution time: {time() - start}s")
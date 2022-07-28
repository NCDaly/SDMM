import matplotlib.pyplot as pyplot
import subprocess
import random
import math
import time
import os

MERSENNE_PRIME = pow(2, 31) - 1

RESULTS_FILE = "results.txt"

X_VALS = ["sdmm_final", "sdmm_final2"]
D_VALS = [128, 256, 512, 1024, 2048]
N_VALS = [16, 32, 48, 64]
S_VALS = [2, 4, 8, 16]
P_VALS = [2, 4, 8, 16]

PATTERNS = ["d", "ps", "sp"]

def randmod():

    return random.randrange(MERSENNE_PRIME)

def progressBar(prog):

    length = 50
    if prog > 0:
        print(u"\u001b[1A\u001b[0K", end="")
    print(f"[{'=' * int(prog * length):<{length}}] {round(prog * 100.0, 1):>5}%")

class Matrix:

    def __init__(self, rows, cols, data=None):

        self.rows = rows
        self.cols = cols
        if data:
            self.data = [[data[i][j] for j in range(cols)] for i in range(rows)]
        else:
            self.data = [[randmod() for j in range(cols)] for i in range(rows)]

    def __mul__(self, other):
        
        if isinstance(other, Matrix):
            rows = self.rows
            cols = other.cols
            if self.cols == other.rows:
                diag = self.cols
            else:
                raise ValueError("Mismatched dimensions")
            
            print(f"Mutiplying matrices ({rows}x{diag}x{cols})...")
            data = [[0 for j in range(cols)] for i in range(rows)]
            elems = 0.0
            total = 1.0 * rows * cols
            progressBar(0)
            for i in range(rows):
                for j in range(cols):
                    for k in range(diag):
                        data[i][j] += self.data[i][k] * other.data[k][j]
                    data[i][j] %= MERSENNE_PRIME
                    elems += 1.0
                progressBar(elems / total)
            return Matrix(rows, cols, data)
        
        elif isinstance(other, int):
            rows = self.rows
            cols = self.cols
            data = [[self.data[i][j] for j in range(cols)] for i in range(rows)]
            for i in range(rows):
                for j in range(cols):
                    data[i][j] *= other
                    data[i][j] %= MERSENNE_PRIME

            return Matrix(rows, cols, data)
                    
        else:
            raise TypeError("Expected type Matrix or int")

    def __eq__(self, other):

        if not isinstance(other, Matrix):
            return False
        if self.rows != other.rows:
            return False
        if self.cols != other.cols:
            return False
        for i in range(self.rows):
            for j in range(self.cols):
                if self.data[i][j] != other.data[i][j]:
                    return False
        return True

    def __repr__(self):

        return "\n".join([" ".join([str(elem) for elem in row]) for row in self.data])

def importMatrix(filename):

    with open(filename, "r") as file:
        rows = int(file.readline())
        cols = int(file.readline())
        data = [[int(elem) for elem in file.readline().split()] for i in range(rows)]
        return Matrix(rows, cols, data)

def exportMatrix(matrix, filename):

    with open(filename, "w") as file:
        file.write(f"{matrix.rows}\n{matrix.cols}\n")
        file.write(str(matrix))

def precompute(dim, check):

    path = f"test/test{dim}"
    try:        
        os.makedirs(path)
        # Only do computation if the dir doesn't exist
        A = Matrix(dim, dim)
        B = Matrix(dim, dim)
        exportMatrix(A, f"{path}/A.txt")
        exportMatrix(B, f"{path}/B.txt")
        C = A * B if check else Matrix(0, 0)
        exportMatrix(C, f"{path}/C.txt")
    except FileExistsError:
        # Looks like it's already been done!
        pass
    except Exception as e:
        rmdir(path)
        raise e from None

def getRuntime(stdout):

    lines = stdout.split("\n");
    return float(lines[0].split()[-1])

def runTest(program, dimension, numPEs, numSplits, numPrevent):

    # Create command
    path = f"test/test{dimension}"
    command = [
        "oshrun",
        "-np",
        str(numPEs),
        program,
        str(numSplits),
        str(numPrevent),
        f"{path}/A.txt",
        f"{path}/B.txt",
        f"{path}/out.txt",
    ]
    
    # Run C program and capture output
    print("$", *command)
    output = subprocess.run(command, stdout=subprocess.PIPE);
    stdout = output.stdout.decode()
    runtime = getRuntime(stdout)
    print(stdout)

    # Check multiplication
    ans = importMatrix(f"{path}/C.txt")
    out = importMatrix(f"{path}/out.txt")
    if out == ans:
        result = "PASS"
    elif ans == Matrix(0, 0):
        result = "NONE"
    else:
        result = "FAIL"

    # Write results to a file
    info = [
        f"{program:<16}"
        f"D={path[9:]:<4}",
        f"N={numPEs:<2}",
        f"S={numSplits:<2}",
        f"P={numPrevent:<2}",
        result,
        f"{round(runtime, 6):>10}s"
    ]
    with open(RESULTS_FILE, "a") as file:
        file.write(" ".join(info) + "\n")

def importData():

    data = {}
    with open(RESULTS_FILE, "r") as file:
        for line in file:
            results = line.split()
            x = results[0];
            d = int(results[1][2:])
            n = int(results[2][2:])
            s = int(results[3][2:])
            p = int(results[4][2:])
            t = float(results[6][:-1])
            data[(x, d, n, s, p)] = t
    
    return data        
        
def makePlots(pattern, data):

    pattern = f"{pattern.upper():X<2}"
    path = f"plot/plot{pattern}"

    # Ensure we have a directory to put our files in
    os.makedirs(path, exist_ok=True)

    symbols = "XDNSP"
    for symbol in symbols:
        if symbol not in pattern:
            pattern += symbol

    i0 = symbols.index(pattern[0])
    i1 = symbols.index(pattern[1])
    i2 = symbols.index(pattern[2])
    i3 = symbols.index(pattern[3])
    i4 = symbols.index(pattern[4])
    
    vals = [X_VALS, D_VALS, N_VALS, S_VALS, P_VALS]
    labels = [
        "Execute",
        "Dimension",
        "Nodes",
        "Splits",
        "Prevent"]
    val = [0, 0, 64, 0, 0]

    plots = 0.0
    total = len(vals[i2]) * len(vals[i3]) * len(vals[i4])
    print(f"Generating plots: {pattern[:2]}...")
    progressBar(0.0)
    for val[i4] in vals[i4]:
        for val[i3] in vals[i3]:
            for val[i2] in vals[i2]:
                if val[2] < 2 * (val[3] + val[4]): # Ensure we have enough nodes
                    plots += 1
                    continue

                for val[i1] in vals[i1]:
                    if val[2] < 2 * (val[3] + val[4]): # Ensure we have enough nodes
                        continue

                    pointsX = []
                    pointsY = []
                    for val[i0] in vals[i0]:
                        if val[2] < 2 * (val[3] + val[4]): # Ensure we have enough nodes
                            continue

                        pointsX.append(val[i0])
                        pointsY.append(data[tuple(val)])
                    pyplot.plot(pointsX, pointsY, label=f"{labels[i1]} = {val[i1]}")

                # Make the plot, now that we have the data
                pyplot.xlabel(f"{labels[i0]}")
                pyplot.ylabel("Runtime (s)")
                pyplot.title(f"SDMM Runtime vs. {labels[i0]}\n" + \
                             f"{labels[i2]} = {val[i2]}, " + \
                             f"{labels[i3]} = {val[i3]}, " + \
                             f"{labels[i4]} = {val[i4]}")
                pyplot.legend()
                #pyplot.show()
                fname = "_".join((
                    f"plot{pattern[:2]}",
                    f"{pattern[2]}{val[i2]}",
                    f"{pattern[3]}{val[i3]}",
                    f"{pattern[4]}{val[i4]}"))
                pyplot.savefig(f"{path}/{fname}.png")
                pyplot.clf()
                plots += 1
                progressBar(plots / total)
        
def main():

    # Clear results file
    if input("Clear previous results? [y/N] ").lower() == "y":
        with open(RESULTS_FILE, "w") as file:
            pass

    # Precompute results
    check = (input("Check for correctness? [y/N] ").lower() == "y")
    for d in D_VALS:
        precompute(d, check)
    
    # Run lots of tests
    for x in X_VALS:
        if input(f"Test program {x}? [y/N] ").lower() == "y":
            for d in D_VALS:
                for n in N_VALS:
                    for s in S_VALS:
                        for p in P_VALS:
                            if n >= 2 * (s + p): # Ensure we have enough nodes
                                runTest(x, d, n, s, p)
                        
    # Make lots of graphs
    data = importData()
    for pattern in PATTERNS:
        makePlots(pattern, data)
    
if __name__ == "__main__":
    main()

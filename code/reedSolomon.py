import numpy

def encode(msg):
    msg = msg.upper()
    l = len(msg)
    return [sum([(ord(msg[i]) - ord("A")) * pow(x, i) for i in range(l)]) for x in range(l)]

def decode(pts, idxA=0):
    msg = solve(pts)
    return "".join([chr(msg[i] + ord("A") - idxA) for i in range(len(pts))])

def solve(pts):
    l = len(pts)
    mat = numpy.array([[pow(x,i)for i in range(l)]for x in range(l)])
    ans = numpy.array(pts)
    return [round(x) for x in numpy.linalg.inv(mat).dot(ans)]

print(encode("bailey"))
print(decode([2,46,1184,11408,60190,222362,651356]))

from numpy import sin, cos, pi
from scipy.integrate import odeint

def model(z, t):
    theta = z[2]
    return [cos(theta), sin(theta), 0.2*pi]

z0 = [0, 0, 0]
t = [0, 0.95]
sol = odeint(model, z0, t)

print(sol)
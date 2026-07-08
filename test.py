import flooride

# Load
flooride.mat.lay("test.flrd")
print(flooride.mat.here("obama"))

# Inspect
print(flooride.mat.inspect())

# Create and test object
m = flooride.mat("hello", ["world"], [0.1, 0.2, 0.3])
print(m.see())

m.restyle('w', 'goodbye', -1)
m.restyle('n', 'friend', 0)
m.restyle('e', '9.9', 0)
print(m.see())

# Roll
flooride.mat.roll("out.flrd")
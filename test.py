import flooride

flooride.mat.lay("out.flrd")
word = flooride.mat.MakeMat("kind", [], [457,31,-33])
print(word.GetWord())
print(word.GetNext())
print(word.see())
print(flooride.mat.inspect())
flooride.mat.paint("d", 3)
print(flooride.mat.inspect())

print(flooride.mat.here("obama"))

flooride.mat.roll("out.flrd")
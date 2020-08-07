The syntax to run the code is:

	AcquMC	par/G4He_G4He.dat

and obviously you need to modify the parameters in the parameter file to change what's going on.

The three parameter files that should be ready to go are the Compton one:

	G4He_G4He.dat

the pi0 one:

	G4He_4HePi0.dat

and the helium disintegration one:

	G4He_DPN.dat

The name scheme is "gamma + 4He -> 4He + Pi0", for example.

The only things you should modify are the number of events ("Number-Throws"),
the beam energy ("P-Distr: Beam T-Dist 1/x") in GeV, and the output file name
("Ntuple-Output").  I wouldn't mess with anything else unless you know what you're doing.

You can check these results of the first two reactions with the output
produced by EvGen, and then you can play around with more complicated, 3-body
reactions like "gamma + 4He -> d + p + n".  In order to do that, though, you
will have to run everything through A2Geant4 and then AcquRoot since the
output of AcquMC doesn't have the nice histograms like EvGen.

If you have any questions let me know.

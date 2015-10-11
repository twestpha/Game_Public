# X3D_to_Map.py
# uses python3
# Trevor Westphal
# converts *.x3d scenes to the custom *.map files
#
# Map file Specification
#
# First, the camera coordinates
# This single line begins with a "v" tag, for camera
# This is in the format:
# v x, y position
# for example
# v 0.0 0.0
#
# Next, the list of unique objects in the scene
# These start with the "m" tag, for mesh
# These are found assuming they are .obj files
# For example:
# m cube.obj
#
# Then the list of unique textures in the scene
# These start with the "t" tag, for texture
# These are found assuming the following format
# - If it is a diffuse map, (object name)_diff.png
# - If it is a specular map, (object name)_spec.png
# - If it is a normal map, (object name)_norm.png
# - If it is an emissive map, (object name)_emit.png
# We will not look for jpg/jpeg. Those are stupid and should be hated.
# For example:
# t cube_diff.png
# t cube_spec.png
# t fire_part_diff.png
#
# Next, the application of certain textures to certain models, and location
# This is a longer, more complex field, denoted with the "d" tag, for drawable
# The first entry is the index of the object in the above list of objects
# The next entries are texture entries. These are the textures that will be applied to the model
# IMPORTANT! Texture Indices begin with 1-based indexing!
# They are ALWAYS in the order
# DIFF SPEC NORM EMIT
# the argument 0 denotes No Texture.
# The next three entries after the textures are the x, y, and z position
# Then the scale
# Then the x, y, and z rotation
# For example:
# d 0 1 2 0 0 0.0 0.0 0.0 1.0 0.0 0.0 0.0
#
# Next is the field that denotes additional textures to be loaded as animated or layered textures
# Theses are denoted with the "c" tag, for cyclic textures. They will consist of a tag, then the
# indices of the texture references in the "DIFF SPEC NORM EMIT" pattern
# For example:
# c 3 0 0 0
#
# After that is the particle points. These are denoted with a "p" tag. Within the X3D file,
# particles are denoted by being one of the pre-programmed particles, such as
#   - Snow
#   - Fire
#   - Smoke
#   - etc.
# Following this will be the x, y, and z positions.
# For example:
# p fire 0.0 0.0 4.3
#
# This next section detailing the ground MUST be handwritten by the user!
# Next, come the ground specifications. This will be a list of images that denote texturing of the ground.
# This will first list the heightmap. This black-and-white image will contain the data for generating the mesh
# of the ground. Then, a series of 8 pairs of images will follow. These will be in a certain pattern. The first of the
# pair will be the actual texture to load onto the graphics card and draw on the ground. The second will be the grayscale
# mask. A white pixel here will result in a fully opaque texture on the ground mesh.
# These will be denoted by the tags "h" and "g" respectively. (H-> Heightmap, G->ground texture)
# for example:
# h heightmap.png
# g tile_texture_1.png 1_mask.png
# g tile_texture_2.png 2_mask.png

import sys
import os
from math import *

class ObjectReference:

    StaticUniqueObjectsList = []
    StaticUniqueTexturesList = []

    def __init__(self, name, x, y, z, scale, x_rot, y_rot, z_rot, angle):
        self.x3dName = name
        if any(char.isdigit() for char in name):
            self.x3dName = name[:-4]

        self.matchedName = ""
        self.matchedDiffTexName = ""
        self.matchedSpecTexName = ""
        self.matchedNormTexName = ""
        self.matchedEmitTexName = ""

        self.x_pos = x
        self.y_pos = y
        self.z_pos = z

        self.scale = scale

        self.x_rot = x_rot
        self.y_rot = y_rot
        self.z_rot = z_rot
        self.angle = angle

    def getDescriptor(self):
        if(self.matchedName != ""):
            try:
                name = self.StaticUniqueObjectsList.index(self.matchedName)
            except:
                name = 0
        else:
            name = 0

        if(self.matchedDiffTexName != ""):
            try:
                diff = self.StaticUniqueTexturesList.index(self.matchedDiffTexName)
            except:
                diff = -1
        else:
            diff = -1

        if(self.matchedSpecTexName != ""):
            try:
                spec = self.StaticUniqueTexturesList.index(self.matchedSpecTexName)
            except:
                spec = -1
        else:
            spec = -1

        if(self.matchedNormTexName != ""):
            try:
                norm = self.StaticUniqueTexturesList.index(self.matchedNormTexName)
            except:
                norm = -1
        else:
            norm = -1

        if(self.matchedEmitTexName != ""):
            try:
                emit = self.StaticUniqueTexturesList.index(self.matchedEmitTexName)
            except:
                emit = -1
        else:
            emit = -1

        x = self.x_rot
        y = self.y_rot
        z = self.z_rot
        angle = self.angle

        print "Rotation around <%.2f, %.2f, %.2f> = %.2f for %s" % (x, y, z, angle, self.matchedName)

        # This is be fuckity
        x_rotation = asin(x * y * (1 - cos(angle)) + z * sin(angle))
        y_rotation = atan2(y * sin(angle)- x * z * (1 - cos(angle)) , 1 - (y*y + z*z ) * (1 - cos(angle)))
        z_rotation = atan2(x * sin(angle)-y * z * (1 - cos(angle)) , 1 - (x*x + z*z) * (1 - cos(angle))) + pi / 2.0

        # Add 1 to texture for one-based indexing
        return ("d %s %s %s %s %s %f %f %f %f %f %f %f %f\n" % (name, diff+1, spec+1, norm+1, emit+1, self.x_pos, self.y_pos, self.z_pos, self.scale, x, y, z, angle))

    def getParticleDescriptor(self):
        return ("p %s %f %f %f\n" % (self.x3dName[:-5], self.x_pos, self.y_pos, self.z_pos))

class Converter:
    def __init__(self):
        self.searchForObjects = False
        self.searchForTextures = False
        self.inputFileName = ""
        self.outputFileName = ""

    def convertFile(self, arguments):
        self.parseArguments(arguments)
        self.scanFile()
        if(self.searchForObjects):
            self.attachObjects()
        if(self.searchForTextures):
            self.attachTextures()
        self.writeToFile()

    def parseArguments(self, arguments):
        if(len(arguments)>5 or len(arguments)<4):
            self.printUsage()
            exit(1)

        self.inputFileName = arguments[1]
        self.outputFileName = arguments[2]

        option1 = arguments[3]
        option2 = ""

        try:
            option2 = arguments[4]
        except:
            pass

        if(option1 == "-d" or option2 == "-d"):
            return
        if(option1 == "-so" or option2 == "-so"):
            self.searchForObjects = True
        if(option1 == "-st" or option2 == "-st"):
            self.searchForTextures = True

    def printUsage(self):
        print("\nX3D_to_Map usage:\npython3 X3D_to_Map.py [input file] [output file] [argument list]\n\n -so\n\tEnable searching for related objects\n -st\n\tEnable searching for related texture images\n -d\n\tIgnore related files and write defaults\n\tThis overrides any other options\n")

    def scanFile(self):
        ifile = open(self.inputFileName, "r")

        self.objectList = []

        linesLeft = 0

        for line in ifile:
            line = line.strip()
            if "<Transform DEF=\"" in line:
                if "ifs" not in line:
                    linesLeft = 4
                    line = line.split("\"")[1]
                    if line.endswith("_TRANSFORM"):
                        line = line[:-10]
                    tempName = line

            if(linesLeft > 0):

                # print(line)

                if "translation" in line:
                    line = line.split("\"")[1]
                    line = line.split(" ")
                    tempX = float(line[0])
                    tempY = float(line[1])
                    tempZ = float(line[2])
                elif "scale" in line:
                    line = line.split("\"")[1]
                    line = line.split(" ")
                    tempS = float(line[0])
                elif "rotation" in line:
                    line = line.split("\"")[1]
                    line = line.split(" ")
                    tempA = float(line[0])
                    tempB = float(line[1])
                    tempC = float(line[2])
                    tempAngle = float(line[3])
                linesLeft-=1

                if(linesLeft==0):
                    Obj = ObjectReference(tempName, tempX, tempY, tempZ, tempS, tempA, tempB, tempC, tempAngle)
                    self.objectList.append(Obj);

        ifile.close()


    def attachObjects(self):
        # print("Attaching Objects:")
        for obj in self.objectList:
            for root, dirs, files in os.walk("../res/models/"):
                for file in files:
                    potential_filename = os.path.join(root, file).split("/")[-1]
                    if(potential_filename == obj.x3dName+".dae"):
                        obj.matchedName = obj.x3dName+".dae"
                        # print(obj.matchedName)


    def attachTextures(self):
        # print("Attaching textures:")
        for obj in self.objectList:
            for root, dirs, files in os.walk("../res/textures/"):
                for file in files:
                    potential_filename = os.path.join(root, file).split("/")[-1]
                    if(potential_filename == obj.x3dName+"_diff.png"):
                        obj.matchedDiffTexName = obj.x3dName+"_diff.png"
                        # print(potential_filename)
                    elif(potential_filename == obj.x3dName+"_spec.png"):
                        obj.matchedSpecTexName = obj.x3dName+"_spec.png"
                        # print(potential_filename)
                    elif(potential_filename == obj.x3dName+"_norm.png"):
                        obj.matchedNormTexName = obj.x3dName+"_norm.png"
                        # print(potential_filename)
                    elif(potential_filename == obj.x3dName+"_emit.png"):
                        obj.matchedEmitTexName = obj.x3dName+"_emit.png"
                        # print(potential_filename)

    def writeToFile(self):
        UniqueObjectsList = []
        UniqueTexturesList = []

        for obj in self.objectList:
            if(obj.matchedName != "" and obj.matchedName not in UniqueObjectsList):
                UniqueObjectsList.append(obj.matchedName)

            if(obj.matchedDiffTexName != "" and obj.matchedDiffTexName not in UniqueTexturesList):
                UniqueTexturesList.append(obj.matchedDiffTexName)

            if(obj.matchedSpecTexName != "" and obj.matchedSpecTexName not in UniqueTexturesList):
                UniqueTexturesList.append(obj.matchedSpecTexName)

            if(obj.matchedNormTexName != "" and obj.matchedNormTexName not in UniqueTexturesList):
                UniqueTexturesList.append(obj.matchedNormTexName)

            if(obj.matchedEmitTexName != "" and obj.matchedEmitTexName not in UniqueTexturesList):
                UniqueTexturesList.append(obj.matchedEmitTexName)

        ObjectReference.StaticUniqueTexturesList = UniqueTexturesList
        ObjectReference.StaticUniqueObjectsList  = UniqueObjectsList

        ofile = open(self.outputFileName, "w")

        ofile.write("# Converted .map file\n\n")

        ofile.write("# Camera declaration\nc 0.0f, 0.0\n")

        ofile.write("\n# Included Meshes (m)\n")

        for element in UniqueObjectsList:
            ofile.write("m %s\n" % element)

        ofile.write("\n# Included Textures (t)\n")

        for element in UniqueTexturesList:
            ofile.write("t %s\n" % element)

        ofile.write("\n# Object location, rotation, scale, and texture binding (d)\n")

        for obj in self.objectList:
            if "_part" not in obj.x3dName:
                ofile.write(obj.getDescriptor())

        ofile.write("\n# Particle definitions (p)\n")

        for obj in self.objectList:
            if "_part" in obj.x3dName:
                ofile.write(obj.getParticleDescriptor())

        ofile.write("\n# Ground specification - Begin writing here (h, g)\n")

        ofile.close()

C = Converter()
C.convertFile(sys.argv)

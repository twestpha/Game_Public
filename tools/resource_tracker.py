# resource_tracker.py
# uses python3
# Trevor Westphal

# Walk through /res/maps and res/layouts
# collect unique models and textures from both

# Walk through res/textures
# If there needs to be one, print a warning
# if there is an extra one, "   
# Repeat for models

WARNING = '\033[93m'
ERROR = '\033[91m'
ENDC = '\033[0m'

import os
import re

class Tracker:

    StaticScrapedUniqueModelsList = []
    StaticScrapedUniqueTexturesList = []

    StaticDetectedUniqueModelsList = []
    StaticDetectedUniqueTexturesList = []

    def walkDirectoryAndScrapeFiles(self, path):
        for root, dirs, files in os.walk(path):
                for file in files:
                    self.parse(root + "/" + file)

    def parse(self, file_name):

        file = open(file_name, 'r')

        for line in file:
            if ".png" in line:
                match = re.search('[A-Za-z_]+\.png', line)
                if match and match.group(0) not in self.StaticScrapedUniqueTexturesList:
                    self.StaticScrapedUniqueTexturesList.append((match.group(0), file_name))

            if ".dae" in line:
                match = re.search('[A-Za-z_]+\.dae', line)
                if match and match.group(0) not in self.StaticScrapedUniqueModelsList:
                    self.StaticScrapedUniqueModelsList.append((match.group(0), file_name))

    def walkDirectoryAndDetectResources(self, path):
         for root, dirs, files in os.walk(path):
                for file in files:
                    if ".png" in file and file not in self.StaticDetectedUniqueTexturesList:
                        self.StaticDetectedUniqueTexturesList.append(file)

                    if ".dae" in file and file not in self.StaticDetectedUniqueModelsList:
                        self.StaticDetectedUniqueModelsList.append(file)

    def compareTextures(self):
        scraped_texture_list = []

        for texture in self.StaticScrapedUniqueTexturesList:
            scraped_texture_list.append(texture[0])
            if texture[0] not in self.StaticDetectedUniqueTexturesList:
                self.printError("Texture '%s' found in '%s' doesn't exist anywhere." % (texture[0], texture[1]))

        print("")

        for texture in self.StaticDetectedUniqueTexturesList:
            if texture not in scraped_texture_list:
                self.printWarning("Texture '%s' is not referenced anywhere." % texture)

    def compareModels(self):
        scraped_Model_list = []

        for model in self.StaticScrapedUniqueModelsList:
            scraped_Model_list.append(model[0])
            if model[0] not in self.StaticDetectedUniqueModelsList:
                self.printError("Model '%s' found in '%s' doesn't exist anywhere." % (model[0], model[1]))

        print("")

        for model in self.StaticDetectedUniqueModelsList:
            if model not in scraped_Model_list:
                self.printWarning("Model '%s' is not referenced anywhere." % model)
    

    def printError(self, message):
        print("%s[Error]%s %s" % (ERROR, ENDC, message))

    def printWarning(self, message):
        print("%s[Warning]%s %s" % (WARNING, ENDC, message))



t = Tracker()
t.walkDirectoryAndScrapeFiles("../res/layouts")
t.walkDirectoryAndScrapeFiles("../res/maps")
t.walkDirectoryAndScrapeFiles("../src")

t.walkDirectoryAndDetectResources("../res/models")
t.walkDirectoryAndDetectResources("../res/fonts")
t.walkDirectoryAndDetectResources("../res/textures")

t.compareTextures()
print("")
t.compareModels()

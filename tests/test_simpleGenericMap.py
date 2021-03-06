# This file is part of afw.
#
# Developed for the LSST Data Management System.
# This product includes software developed by the LSST Project
# (https://www.lsst.org).
# See the COPYRIGHT file at the top-level directory of this distribution
# for details of code ownership.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

from collections.abc import MutableMapping
import unittest

import lsst.utils.tests

from lsst.afw.typehandling import SimpleGenericMap
from lsst.afw.typehandling.testUtils import MutableGenericMapTestBaseClass


class SimpleGenericMapTestSuite(MutableGenericMapTestBaseClass):

    @staticmethod
    def makeMap(mapType, values):
        """Initialize a map type using __setattr__ instead of a bulk constructor.
        """
        result = mapType()
        for key, value in values.items():
            result[key] = value
        return result

    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        cls.genericConstructor = SimpleGenericMap  # Must be mapping-constructible
        cls.targets = {SimpleGenericMap[str]}  # Must be default-constructible
        cls.examples = {
            "SimpleGenericMap(testData(str))": (SimpleGenericMap[str], cls.getTestData(str)),
            "SimpleGenericMap(testKeys(str) : 0)":
                (SimpleGenericMap[str], {key: 0 for key in cls.getTestData(str).keys()}),
            "SimpleGenericMap(dtype=str)": (SimpleGenericMap[str], {}),
        }

    def tearDown(self):
        pass

    def testClass(self):
        for target in self.targets:
            self.assertTrue(issubclass(target, MutableMapping))
            self.assertIsInstance(target(), MutableMapping)

    def testInitKeywords(self):
        for target in self.targets:
            for keyType in self.getValidKeys(target):
                self.checkInitKwargs(target, self.getTestData(keyType), msg=str(target))

    def testInitPairs(self):
        for target in self.targets | {self.genericConstructor}:
            for keyType in self.getValidKeys(target):
                self.checkInitPairs(target, self.getTestData(keyType), msg=str(target))

    def testInitMapping(self):
        for target in self.targets | {self.genericConstructor}:
            for keyType in self.getValidKeys(target):
                # Init from dict
                self.checkInitMapping(target, self.getTestData(keyType), msg=str(target))
                # Init from GenericMap
                self.checkInitMapping(target, target(self.getTestData(keyType)),
                                      msg=str(target))

    def testUnknownKeys(self):
        with self.assertRaises(TypeError):
            self.genericConstructor()
        # Should not raise
        self.genericConstructor(dtype=str)

    def testMixedKeys(self):
        badData = {"What do you get if you multiply six by nine?": "Ultimate Question",
                   42: "Ultimate Answer",
                   }
        for target in self.targets | {self.genericConstructor}:
            with self.assertRaises(TypeError):
                target(badData)
            with self.assertRaises(TypeError):
                target(badData.items())
        for target in self.targets:
            with self.assertRaises(TypeError):
                target(**badData)

    def testFromKeys(self):
        for target in self.targets:
            for keyType in self.getValidKeys(target):
                keys = self.getTestData(keyType).keys()
                for value in self.getTestData(keyType).values():
                    self.checkFromKeys(target, keys, value,
                                       msg=" class=%s, value=%r" % (target, value))
                self.checkFromKeysDefault(target, keys, msg=" class=%s, no value" % (target))

    def testCopy(self):
        for label, (mappingType, contents) in self.examples.items():
            for keyType in self.getValidKeys(mappingType):
                mapping1 = self.makeMap(mappingType, contents)
                mapping2 = mapping1.copy()
                self.assertEqual(mapping1, mapping2, msg="%s" % label)
                mapping1[keyType(42)] = "A random value!"
                self.assertNotEqual(mapping1, mapping2, msg="%s" % label)

    def testEquality(self):
        for label1, (mappingType1, contents1) in self.examples.items():
            mapping1 = self.makeMap(mappingType1, contents1)
            for label2, (mappingType2, contents2) in self.examples.items():
                mapping2 = self.makeMap(mappingType2, contents2)
                if contents1 == contents2:
                    self.assertIsNot(mapping1, mapping2, msg="%s vs %s" % (label1, label2))
                    self.assertEqual(mapping1, mapping2, msg="%s vs %s" % (label1, label2))
                    self.assertEqual(mapping1, contents2, msg="%s vs dict(%s)" % (label1, label2))
                    self.assertEqual(contents1, mapping2, msg="dict(%s) vs %s" % (label1, label2))
                else:
                    self.assertNotEqual(mapping1, mapping2, msg="%s vs %s" % (label1, label2))
                    self.assertNotEqual(mapping1, contents2, msg="%s vs dict(%s)" % (label1, label2))
                    self.assertNotEqual(contents1, mapping2, msg="dict(%s) vs %s" % (label1, label2))

    def testBool(self):
        for label, (mappingType, contents) in self.examples.items():
            mapping = self.makeMap(mappingType, contents)
            if contents:
                self.assertTrue(mapping, msg=label)
            else:
                self.assertFalse(mapping, msg=label)

    def testContains(self):
        for label, (mappingType, contents) in self.examples.items():
            mapping = self.makeMap(mappingType, contents)
            self.checkContains(mapping, contents, msg=label)

    def testContents(self):
        for label, (mappingType, contents) in self.examples.items():
            mapping = self.makeMap(mappingType, contents)
            self.checkContents(mapping, contents, msg=label)

    def testGet(self):
        for label, (mappingType, contents) in self.examples.items():
            mapping = self.makeMap(mappingType, contents)
            self.checkGet(mapping, contents, msg=label)

    def testIteration(self):
        for label, (mappingType, contents) in self.examples.items():
            mapping = self.makeMap(mappingType, contents)
            self.checkIteration(mapping, contents, msg=label)

    def testViews(self):
        for label, (mappingType, contents) in self.examples.items():
            self.checkMutableViews(mappingType, contents, msg=label)

    def testInsertItem(self):
        for target in self.targets:
            for keyType in self.getValidKeys(target):
                self.checkInsertItem(target, self.getTestData(keyType), msg=str(target))

    def testSetdefault(self):
        for target in self.targets:
            for keyType in self.getValidKeys(target):
                self.checkSetdefault(target, self.getTestData(keyType), msg=str(target))

    def testUpdateMapping(self):
        for target in self.targets:
            for keyType in self.getValidKeys(target):
                # Update from dict
                self.checkUpdateMapping(target, self.getTestData(keyType), msg=str(target))
                # Update from GenericMap
                self.checkUpdateMapping(target, self.makeMap(target, self.getTestData(keyType)),
                                        msg=str(target))

    def testUpdatePairs(self):
        for target in self.targets:
            for keyType in self.getValidKeys(target):
                self.checkUpdatePairs(target, self.getTestData(keyType), msg=str(target))

    def testUpdateKwargs(self):
        for target in self.targets:
            for keyType in self.getValidKeys(target):
                self.checkUpdateKwargs(target, self.getTestData(keyType), msg=str(target))

    def testReplaceItem(self):
        for target in self.targets:
            self.checkReplaceItem(target(), msg=str(target))

    def testRemoveItem(self):
        for target in self.targets:
            for keyType in self.getValidKeys(target):
                self.checkRemoveItem(target, self.getTestData(keyType), msg=str(target))

    def testPop(self):
        for target in self.targets:
            for keyType in self.getValidKeys(target):
                self.checkPop(target, self.getTestData(keyType), msg=str(target))

    def testPopitem(self):
        for target in self.targets:
            for keyType in self.getValidKeys(target):
                self.checkPopitem(target, self.getTestData(keyType), msg=str(target))

    def testClear(self):
        for target in self.targets:
            for keyType in self.getValidKeys(target):
                self.checkClear(target, self.getTestData(keyType), msg=str(target))


class MemoryTester(lsst.utils.tests.MemoryTestCase):
    pass


def setup_module(module):
    lsst.utils.tests.init()


if __name__ == "__main__":
    lsst.utils.tests.init()
    unittest.main()

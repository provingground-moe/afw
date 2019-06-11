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

__all__ = ["PointKey", "CovarianceMatrixKey"]

import numpy as np

from lsst.utils import TemplateMeta
from . import aggregates


class PointKey(metaclass=TemplateMeta):
    TEMPLATE_PARAMS = ("dtype", "dim")
    TEMPLATE_DEFAULTS = (None, 2)


PointKey.register((np.int32, 2), aggregates.Point2IKey)
PointKey.register((np.float64, 2), aggregates.Point2DKey)


# Because Boxes aren't templates themselves, we don't expose the fact that
# BoxKey is a template to the Python user; it's considered an implementation
# detail.


class CovarianceMatrixKey(metaclass=TemplateMeta):
    TEMPLATE_PARAMS = ("dtype", "dim")


CovarianceMatrixKey.register((np.float32, 2), aggregates.CovarianceMatrix2fKey)
CovarianceMatrixKey.register((np.float32, 3), aggregates.CovarianceMatrix3fKey)
CovarianceMatrixKey.register((np.float32, 4), aggregates.CovarianceMatrix4fKey)
CovarianceMatrixKey.register(
    (np.float32, None), aggregates.CovarianceMatrixXfKey)
CovarianceMatrixKey.register((np.float64, 2), aggregates.CovarianceMatrix2dKey)
CovarianceMatrixKey.register((np.float64, 3), aggregates.CovarianceMatrix3dKey)
CovarianceMatrixKey.register((np.float64, 4), aggregates.CovarianceMatrix4dKey)
CovarianceMatrixKey.register(
    (np.float64, None), aggregates.CovarianceMatrixXdKey)

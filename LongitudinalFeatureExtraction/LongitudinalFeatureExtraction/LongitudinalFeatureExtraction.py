import os
import unittest
import vtk, qt, ctk, slicer
from slicer.ScriptedLoadableModule import *
import logging
import SimpleITK as sitk
import sitkUtils

#
# LongitudinalFeatureExtraction
#

class LongitudinalFeatureExtraction(ScriptedLoadableModule):
  """Uses ScriptedLoadableModule base class, available at:
  https://github.com/Slicer/Slicer/blob/master/Base/Python/slicer/ScriptedLoadableModule.py
  """

  def __init__(self, parent):
    ScriptedLoadableModule.__init__(self, parent)
    self.parent.title = "LongitudinalFeatureExtraction" # TODO make this more human readable by adding spaces
    self.parent.categories = ["Examples"]
    self.parent.dependencies = []
    self.parent.contributors = ["John Doe (AnyWare Corp.)"] # replace with "Firstname Lastname (Organization)"
    self.parent.helpText = """
    This is an example of scripted loadable module bundled in an extension.
    It performs a simple thresholding on the input volume and optionally captures a screenshot.
    """
    self.parent.acknowledgementText = """
    This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
    and Steve Pieper, Isomics, Inc. and was partially funded by NIH grant 3P41RR013218-12S1.
""" # replace with organization, grant and thanks.

#
# LongitudinalFeatureExtractionWidget
#

class LongitudinalFeatureExtractionWidget(ScriptedLoadableModuleWidget):
  """Uses ScriptedLoadableModuleWidget base class, available at:
  https://github.com/Slicer/Slicer/blob/master/Base/Python/slicer/ScriptedLoadableModule.py
  """

  def setup(self):
    ScriptedLoadableModuleWidget.setup(self)

    # Instantiate and connect widgets ...

    #
    # baseline image Area
    #
    baselineCollapsibleButton = ctk.ctkCollapsibleButton()
    baselineCollapsibleButton.text = "Baseline image"
    self.layout.addWidget(baselineCollapsibleButton)

    # Layout within the dummy collapsible button
    parametersFormLayout = qt.QFormLayout(baselineCollapsibleButton)

    #
    # image (original) volume selector
    #
    self.baselineSelector = slicer.qMRMLNodeComboBox()
    self.baselineSelector.nodeTypes = ["vtkMRMLScalarVolumeNode"]
    self.baselineSelector.selectNodeUponCreation = True
    self.baselineSelector.addEnabled = False
    self.baselineSelector.removeEnabled = False
    self.baselineSelector.noneEnabled = False
    self.baselineSelector.showHidden = False
    self.baselineSelector.showChildNodeTypes = False
    self.baselineSelector.setMRMLScene( slicer.mrmlScene )
    self.baselineSelector.setToolTip( "Pick the image with the original ROI." )
    parametersFormLayout.addRow("Baseline image: ", self.baselineSelector)
    
    #
    # ROI volume selector
    #
    self.roiSelector = slicer.qMRMLNodeComboBox()
    self.roiSelector.nodeTypes = ["vtkMRMLLabelMapVolumeNode"]
    self.roiSelector.selectNodeUponCreation = True
    self.roiSelector.addEnabled = False
    self.roiSelector.removeEnabled = False
    self.roiSelector.noneEnabled = False
    self.roiSelector.showHidden = False
    self.roiSelector.showChildNodeTypes = False
    self.roiSelector.setMRMLScene( slicer.mrmlScene )
    self.roiSelector.setToolTip( "Pick the region of interest." )
    parametersFormLayout.addRow("Region of interest (label map) ", self.roiSelector)
    
    #
    # image2 Area
    #
    image2CollapsibleButton = ctk.ctkCollapsibleButton()
    image2CollapsibleButton.text = "Image set 2"
    self.layout.addWidget(image2CollapsibleButton)

    # Layout within the dummy collapsible button
    parametersFormLayout = qt.QFormLayout(image2CollapsibleButton)
    
    #
    # image2 volume selector
    #
    self.image2Selector = slicer.qMRMLNodeComboBox()
    self.image2Selector.nodeTypes = ["vtkMRMLScalarVolumeNode"]
    self.image2Selector.selectNodeUponCreation = True
    self.image2Selector.addEnabled = False
    self.image2Selector.removeEnabled = False
    self.image2Selector.noneEnabled = False
    self.image2Selector.showHidden = False
    self.image2Selector.showChildNodeTypes = False
    self.image2Selector.setMRMLScene( slicer.mrmlScene )
    self.image2Selector.setToolTip( "Pick the second image." )
    parametersFormLayout.addRow("Image 2: ", self.image2Selector)
    
    #
    # transform selector
    #
    self.transform2Selector = slicer.qMRMLNodeComboBox()
    self.transform2Selector.nodeTypes = ["vtkMRMLTransformNode"]
    self.transform2Selector.selectNodeUponCreation = True
    self.transform2Selector.addEnabled = False
    self.transform2Selector.removeEnabled = False
    self.transform2Selector.noneEnabled = False
    self.transform2Selector.showHidden = False
    self.transform2Selector.showChildNodeTypes = False
    self.transform2Selector.setMRMLScene( slicer.mrmlScene )
    self.transform2Selector.setToolTip( "Pick the transform from image2 to image 1." )
    parametersFormLayout.addRow("Transform: ", self.transform2Selector)
    
    #
    # image3 Area
    #
    image3CollapsibleButton = ctk.ctkCollapsibleButton()
    image3CollapsibleButton.text = "Image set 3"
    self.layout.addWidget(image3CollapsibleButton)

    # Layout within the dummy collapsible button
    parametersFormLayout = qt.QFormLayout(image3CollapsibleButton)
    
    #
    # image2 volume selector
    #
    self.image3Selector = slicer.qMRMLNodeComboBox()
    self.image3Selector.nodeTypes = ["vtkMRMLScalarVolumeNode"]
    self.image3Selector.selectNodeUponCreation = True
    self.image3Selector.addEnabled = False
    self.image3Selector.removeEnabled = False
    self.image3Selector.noneEnabled = True
    self.image3Selector.showHidden = False
    self.image3Selector.showChildNodeTypes = False
    self.image3Selector.setMRMLScene( slicer.mrmlScene )
    self.image3Selector.setToolTip( "Pick the second image." )
    parametersFormLayout.addRow("Image 3: ", self.image3Selector)
    
    #
    # transform selector
    #
    self.transform3Selector = slicer.qMRMLNodeComboBox()
    self.transform3Selector.nodeTypes = ["vtkMRMLTransformNode"]
    self.transform3Selector.selectNodeUponCreation = True
    self.transform3Selector.addEnabled = False
    self.transform3Selector.removeEnabled = False
    self.transform3Selector.noneEnabled = True
    self.transform3Selector.showHidden = False
    self.transform3Selector.showChildNodeTypes = False
    self.transform3Selector.setMRMLScene( slicer.mrmlScene )
    self.transform3Selector.setToolTip( "Pick the transform from image2 to image 1." )
    parametersFormLayout.addRow("Transform: ", self.transform3Selector)
    
    #
    # image4 Area
    #
    image4CollapsibleButton = ctk.ctkCollapsibleButton()
    image4CollapsibleButton.text = "Image set 4"
    self.layout.addWidget(image4CollapsibleButton)

    # Layout within the dummy collapsible button
    parametersFormLayout = qt.QFormLayout(image4CollapsibleButton)
    
    #
    # image2 volume selector
    #
    self.image4Selector = slicer.qMRMLNodeComboBox()
    self.image4Selector.nodeTypes = ["vtkMRMLScalarVolumeNode"]
    self.image4Selector.selectNodeUponCreation = True
    self.image4Selector.addEnabled = False
    self.image4Selector.removeEnabled = False
    self.image4Selector.noneEnabled = True
    self.image4Selector.showHidden = False
    self.image4Selector.showChildNodeTypes = False
    self.image4Selector.setMRMLScene( slicer.mrmlScene )
    self.image4Selector.setToolTip( "Pick the second image." )
    parametersFormLayout.addRow("Image 4: ", self.image4Selector)
    
    #
    # transform selector
    #
    self.transform4Selector = slicer.qMRMLNodeComboBox()
    self.transform4Selector.nodeTypes = ["vtkMRMLTransformNode"]
    self.transform4Selector.selectNodeUponCreation = True
    self.transform4Selector.addEnabled = False
    self.transform4Selector.removeEnabled = False
    self.transform4Selector.noneEnabled = True
    self.transform4Selector.showHidden = False
    self.transform4Selector.showChildNodeTypes = False
    self.transform4Selector.setMRMLScene( slicer.mrmlScene )
    self.transform4Selector.setToolTip( "Pick the transform from image2 to image 1." )
    parametersFormLayout.addRow("Transform: ", self.transform4Selector)

    #
    # Apply Button
    #
    self.applyButton = qt.QPushButton("Apply")
    self.applyButton.toolTip = "Run the algorithm."
    self.applyButton.enabled = True
    parametersFormLayout.addRow(self.applyButton)

    # connections
    self.applyButton.connect('clicked(bool)', self.onApplyButton)

    # Add vertical spacer
    self.layout.addStretch(1)

    # Refresh Apply button state
    self.onSelect()

  def cleanup(self):
    pass

  def onSelect(self):
    self.applyButton.enabled = self.roiSelector.currentNode() and self.baselineSelector.currentNode() and self.image2Selector.currentNode() and self.transform2Selector.currentNode() and self.image3Selector.currentNode() and self.transform3Selector.currentNode() and self.image4Selector.currentNode() and self.transform4Selector.currentNode()

  def onApplyButton(self):
    logic = LongitudinalFeatureExtractionLogic()
    logic.run(self.roiSelector.currentNode(), self.baselineSelector.currentNode(), self.image2Selector.currentNode(), self.transform2Selector.currentNode(), self.image3Selector.currentNode(), self.transform3Selector.currentNode(), self.image4Selector.currentNode(), self.transform4Selector.currentNode())

#
# LongitudinalFeatureExtractionLogic
#

class LongitudinalFeatureExtractionLogic(ScriptedLoadableModuleLogic):
  """This class should implement all the actual
  computation done by your module.  The interface
  should be such that other python code can import
  this class and make use of the functionality without
  requiring an instance of the Widget.
  Uses ScriptedLoadableModuleLogic base class, available at:
  https://github.com/Slicer/Slicer/blob/master/Base/Python/slicer/ScriptedLoadableModule.py
  """

  def hasImageData(self,volumeNode):
    """This is an example logic method that
    returns true if the passed in volume
    node has valid image data
    """
    if not volumeNode:
      logging.debug('hasImageData failed: no volume node')
      return False
    if volumeNode.GetImageData() is None:
      logging.debug('hasImageData failed: no image data in volume node')
      return False
    return True
	
  def invertTransform(self, transform):
    # create new transform and set it as the inverse of the given transform
    inverseTransform = slicer.vtkMRMLTransformNode()
    inverseTransform.SetAndObserveMatrixTransformToParent( transform.GetMatrixTransformFromParent())
    inverseTransform.SetName( transform.GetName() + '-inverse' )
    # push transform to scene
    slicer.mrmlScene.AddNode(inverseTransform)
	
	return
    
  def getLabelStats(self, volume, labelmap):
    # find label value within label map
    imStats = sitk.StatisticsImageFilter()
    imStats.Execute( labelmap )
    label = int(imStats.GetMaximum())
    
    # find the image statistics within the label map
    stats = sitk.LabelStatisticsImageFilter()
    stats.Execute(volume, labelmap)
    individResults = [stats.GetCount(label), stats.GetMean(label), stats.GetVariance(label),stats.GetMaximum(label),stats.GetMinimum(label)]
    
    return individResults
       
  def run(self, labelMap, vol1, vol2, transform):
  
    # create new transform and set it as the inverse of the given transform
    inverseTransform = slicer.vtkMRMLLinearTransformNode()
    inverseTransform.SetAndObserveMatrixTransformToParent( transform.GetMatrixTransformFromParent())
    inverseTransform.SetName( transform.GetName() + '-inverse' )
    # push transform to scene
    slicer.mrmlScene.AddNode(inverseTransform)

    # create new label map (clone volume 1 label map)
    volumesLogic = slicer.modules.volumes.logic()
    labelMap2 = volumesLogic.CloneVolume(labelMap, vol2.GetName() + '-label')
    
    # obtain statistics on vol1 with corresponding label map
    headers = ['Count','Mean','Variance','Maximum','Minimum']
    results=[]
    results.append(headers)
    
    logging.info('Obtaining statistics')
    
    # pull label map from slicer
    lm1 = sitkUtils.PullFromSlicer(labelMap.GetID())
    vl1 = sitkUtils.PullFromSlicer(vol1.GetID())
    results.append(self.getLabelStats(vl1,lm1))
    
    lm2r = None
    
    # harden transform and resample
    transformLogic = slicer.vtkSlicerTransformLogic()
    labelMap2.SetAndObserveTransformNodeID(inverseTransform.GetID())
    transformLogic.hardenTransform(labelMap2)
    #warning = volumesLogic.CheckForLabelVolumeValidity(vol2,labelMap2)
    lm2r = volumesLogic.ResampleVolumeToReferenceVolume( labelMap2, vol2 )
    slicer.mrmlScene.RemoveNode(labelMap2)
    
    # ***************** COPY IN MUCH STUFF FROM LABELSTATISTICS.PY **********************
    
    # pull volume2 and label map from slicer
    lm2 = sitkUtils.PullFromSlicer(lm2r.GetID())
    vl2 = sitkUtils.PullFromSlicer(vol2.GetID())
    
    # apply inverse transform to image and get results
    #lm2r = sitk.Resample(lm2, lm1, inverseTransform, sitk.sitkNearestNeighbor, lm1.GetPixelID())
    results.append(self.getLabelStats(vl2,lm2))
    
    print(results)
    logging.info('Processing completed')
    return True


class LongitudinalFeatureExtractionTest(ScriptedLoadableModuleTest):
  """
  This is the test case for your scripted module.
  Uses ScriptedLoadableModuleTest base class, available at:
  https://github.com/Slicer/Slicer/blob/master/Base/Python/slicer/ScriptedLoadableModule.py
  """

  def setUp(self):
    """ Do whatever is needed to reset the state - typically a scene clear will be enough.
    """
    slicer.mrmlScene.Clear(0)

  def runTest(self):
    """Run as few or as many tests as needed here.
    """
    #self.setUp()
    self.test_LongitudinalFeatureExtraction1()

  def test_LongitudinalFeatureExtraction1(self):
    """ Ideally you should have several levels of tests.  At the lowest level
    tests should exercise the functionality of the logic with different inputs
    (both valid and invalid).  At higher levels your tests should emulate the
    way the user would interact with your code and confirm that it still works
    the way you intended.
    One of the most important features of the tests is that it should alert other
    developers when their changes will have an impact on the behavior of your
    module.  For example, if a developer removes a feature that you depend on,
    your test should break so they know that the feature is needed.
    """

    self.delayDisplay("Starting the test")
    #
    # first, get some data
    #
    import urllib
    downloads = (
        ('http://slicer.kitware.com/midas3/download?items=5767', 'FA.nrrd', slicer.util.loadVolume),
        )

    for url,name,loader in downloads:
      filePath = slicer.app.temporaryPath + '/' + name
      if not os.path.exists(filePath) or os.stat(filePath).st_size == 0:
        logging.info('Requesting download %s from %s...\n' % (name, url))
        urllib.urlretrieve(url, filePath)
      if loader:
        logging.info('Loading %s...' % (name,))
        loader(filePath)
    self.delayDisplay('Finished with download and loading')

    volumeNode = slicer.util.getNode(pattern="FA")
    logic = LongitudinalFeatureExtractionLogic()
    self.assertIsNotNone( logic.hasImageData(volumeNode) )
    self.delayDisplay('Test passed!')

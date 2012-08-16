import sys
import os
import numpy as np


from scipy import sparse
from scipy.sparse import linalg as splinalg


## compute prior 
class PriorGenerator:
    ''' generate prior
    '''
    
    def __init__(self, labelset):
        self.x0     = 0
        #square(x) is useless on binary x...
        # self.x02    = 0
        self.ntrain = 0
        self.mask   = 0
        
        ## intensity prior
        self.im_ntrain = None
        
        self.labelset = np.asarray(labelset)
        
        
    def add_training_data(self, atlas, image=None):
        a = np.asarray(atlas, dtype=int)
        
        ## set unwanted labels to background label (labelset[0])
        bg = self.labelset[0]
        a.flat[~np.in1d(a, self.labelset)] = bg
        
        ## compute background mask
        self.mask = self.mask | (a!=bg)
        
        ## binary assignment matrix
        bin = a.ravel()==np.c_[self.labelset]
        
        ## compute average
        x = bin.astype(float)
        self.x0     = self.x0 + x
        self.ntrain += 1
        
        ## if im is provided, compute average and std of intensity
        # if image is not None:
            # inds = np.where(bin)
            # if self.im_ntrain is None:
                # self.im_avg    = np.sum(image.flat[bin],    axis=1)
                # self.im_avg2   = np.sum(image.flat[bin]**2, axis=1)
                # self.im_ntrain = np.sum(bin,                axis=1)
            # else:
                # self.im_avg    += np.sum(image.flat[bin],    axis=1)
                # self.im_avg2   += np.sum(image.flat[bin]**2, axis=1)
                # self.im_ntrain += np.sum(bin,                axis=1)
                
        
    def get_mask(self):
        return self.mask
        
    def get_prior(self,mask):
        nlabel = len(self.labelset)
        imask = np.where(mask.ravel())[0]
        
        ## average probability
        mean = self.x0[:,imask] / float(self.ntrain)
        
        ## variance
        # x is binary, thus we have avg(x) = avg(x**2)
        var = mean  - mean**2

        ## prior dict
        prior = {
            'labelset': self.labelset,
            'imask':imask, 
            'mean':mean, 
            'var':var,
            }
        
        ## if intensity prior
        if self.im_ntrain is not None:
            im_avg = self.im_avg / self.im_train.astype(float)
            im_var = self.im_avg2 / self.im_train.astype(float) - im_avg**2
            
            ## add to prior dict
            prior['intensity']= (im_avg, im_var)
        
        return prior
        
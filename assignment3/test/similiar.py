from sewar.full_ref import mse, rmse, psnr, uqi, ssim, ergas, scc, rase, sam, msssim, vifp
import cv2
import numpy as np
  
def main():
     original = cv2.imread("res_16.png")
     submit = cv2.imread("submit.png", 1)
     ssim_result = ssim(original, submit)
     rmse_result = rmse(original, submit)
     assert ssim_result[0] > 0.9 and ssim_result[1] > 0.9     
     assert rmse_result <= 50
     print("ssim : ", ssim_result[0] > 0.9 and ssim_result[1] > 0.9)
     print("rmse : ", rmse_result)

       
if __name__ == "__main__":
    main()

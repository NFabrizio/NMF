import numpy as np
import pandas as pd
import sys

def nmf(V, n_components, max_iter=200, tol=1e-4):
    n_rows, n_cols = V.shape
    W = np.random.rand(n_rows, n_components)
    H = np.random.rand(n_components, n_cols)

    for iteration in range(max_iter):
        W_H = np.dot(W, H)
        V_div_W_H = V / (W_H + 1e-10)  # Add small constant to avoid division by zero

        # Update H
        H *= np.dot(W.T, V_div_W_H) / np.dot(W.T, np.ones_like(V))

        # Update W
        W_H = np.dot(W, H)
        V_div_W_H = V / (W_H + 1e-10)  # Add small constant to avoid division by zero
        W *= np.dot(V_div_W_H, H.T) / np.dot(np.ones_like(V), H.T)

        # Calculate Frobenius norm of the residual
        residual = np.linalg.norm(V - np.dot(W, H), 'fro')

        # Check for convergence
        if residual < tol:
            break

        # Calculate and print I-divergence every 500th iteration
        if (iteration + 1) % 500 == 0:
            divergence = np.sum(V * np.log(V / (W_H + 1e-10)) - V + W_H)
            print(f"Iteration {iteration + 1}: I-divergence = {divergence}")

    return W, H

def compare_matrices(V, W, H, threshold):
    # Calculate the reconstructed matrix
    WxH = np.dot(W, H)
    
    # Calculate the element-wise difference between V and WxH
    diff = np.abs(V - WxH)
    
    # Check if all elements in the difference matrix are within the threshold
    return np.all(diff <= threshold)

# a method that will process the command line arguments and call nmf with the given parameters
def main():
    # Read in the command line arguments
    data_file = sys.argv[1]
    iterations = int(sys.argv[2])
    threshold = int(sys.argv[3])
    n_components = int(sys.argv[4])

    # Read the data file into a pandas DataFrame
    df = pd.read_csv(data_file, sep="\t")

    # Extract the data matrix
    data_matrix = df.iloc[:, 1:].values

    # Time the call to nmf and print the time
    import time
    start = time.time()

    # Call nmf with the given parameters
    W, H = nmf(data_matrix, n_components, max_iter=iterations)

    # Stop the timer
    end = time.time()
    print("Time to run python version of nmf: ", end - start)

    # Write W and H to files
    np.savetxt("W.txt", W)
    np.savetxt("H.txt", H)

    is_close = compare_matrices(data_matrix, W, H, threshold)
    print(f"The reconstructed matrix is close to the original: {is_close}")

if __name__ == "__main__":
    main()

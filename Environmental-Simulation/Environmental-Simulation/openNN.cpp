#include "include/MiniDNN.h"
using namespace MiniDNN;

typedef Eigen::MatrixXd Matrix;
typedef Eigen::VectorXd Vector;

// Ignore "libc++abi: terminating with uncaught exception of type std::runtime_error:
// [class Network]: Folder creation failed"

int main()
{
    // Create two dimensional input data
    Vector x0 = Vector::LinSpaced(1000, 0.0, 1.00);
    Vector x1 = Vector::LinSpaced(1000, 0.0, 1.00);
    Vector x2 = Vector::LinSpaced(1000, 0.0, 1.00);
    Vector x3 = Vector::LinSpaced(1000, 0.0, 1.00);
    // Predictors -- each column is an observation
    Matrix x = Matrix::Random(4, 1000);
    x.row(0) = x0;
    x.row(1) = x1;
    x.row(2) = x2;
    x.row(3) = x3;
    // Response variables -- each column is an observation
    Matrix y = Matrix::Random(1, 1000);

    // Fill the output for the training
    for (int i = 0; i < y.cols(); i++)
    {
        y(0, i) = 0.25 * (x(0, i) + x(1, i) + x(2, i) + x(3, i));
    }

    // Fill in test data.
    Matrix xt = (Matrix::Random(4, 1000).array() + 1.0) / 2.0;
    Matrix yt = Matrix::Zero(1, 1000);

    for (int i = 0; i < yt.cols(); i++)
    {
        yt(0, i) = 0.25 * (xt(0, i) + xt(1, i) + xt(2, i) + xt(3, i));
    }

    Network net;

    Layer* layer0 = new FullyConnected<Identity>(4, 200);
    Layer* layer1 = new FullyConnected<ReLU>(200, 200);
    Layer* layer2 = new FullyConnected<ReLU>(200, 200);
    Layer* layer3 = new FullyConnected<Identity>(200, 1);

    // Add layers to the network.
    net.add_layer(layer0);
    net.add_layer(layer1);
    net.add_layer(layer2);
    net.add_layer(layer3);

    net.set_output(new RegressionMSE());

    // Network optimizer.
    Adam opt;
    opt.m_lrate = 0.01;

    VerboseCallback callback;
    net.set_callback(callback);

    // Initialize weights & biases.
    net.init(0, 0.01, 000);

    // Train model.
    net.fit(opt, x, y, 1000, 100, 000);

    // Get matrix of predictions (does not train).
    Matrix pred = net.predict(xt);

    // Save network to disc.
    net.export_net("./Saves/", "Network_File");
    net.export_net("./Saves/", "Backup_Network_File");

    // Rehydrate network.
    Network netFromFile;
    netFromFile.read_net("./Saves/", "Network_File");

    // Make sure it's still the same thing.
    std::cout << (pred - netFromFile.predict(xt)).norm() << std::endl;

    return 0;
}


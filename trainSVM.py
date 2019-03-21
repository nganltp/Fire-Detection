import sys, os
import matplotlib.pyplot as plt
from sklearn import svm
from sklearn.model_selection import train_test_split, GridSearchCV

# # Read data
# x, labels = read_data("points_class_0.txt", "points_class_1.txt")
x=[(-5,-3),(5,6),(10,296),(-90,-10)] 
# data=[img1, img2,img3]
labels=[0,1,1,0]


# Split data to train and test on 80-20 ratio
X_train, X_test, y_train, y_test = train_test_split(x, labels, test_size = 0.2, random_state=0)
 
# Plot traning and test data
# plt.plot(X_train, y_train, X_test, y_test)
plt.plot([20,100],[10,50],'ro')
plt.show()

# Create a linear SVM classifier 
clf = svm.SVC(kernel='linear')

# Train classifier 
clf.fit(X_train, y_train)
 
# Plot decision function on training and test data
# plot_decision_function(X_train, y_train, X_test, y_test, clf)

# Make predictions on unseen test data
clf_predictions = clf.predict(X_test)
print("Accuracy: {}%".format(clf.score(X_test, y_test) * 100 ))

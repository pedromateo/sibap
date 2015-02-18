function click__changeButton()
    __log("Manual Log: clic__changeButton")
    MainWindow.incrementButton:move(5,6)
    MainWindow.stateButton:move(5,40)
    MainWindow.spinBox:move(170,25)
    MainWindow.dialogButton:move(400,25)
    TestDialog.clearButton:move(30,100)
    TestDialog.changeButton:move(200,100)
end

// README.txt 

Student ID: z5102511
Name: Hao Fu

Installation Guide 
- To test this project, you should intsall "Postman" to test this API.
- Following Python libs should be installed.
	* flask
	* json
	* enum
	* flask_restful
	
- Enter the virtual environment by using: " source venv/bin/activate "
- Run "Assignment_1.py" by "python3 Assignment_1.py" or use any Python IDE to open "Assignment_1.py" file.
- The URL (http://127.0.0.1:5000/) will show as an ouput.
- Go to Postman use the URL as a base url to test the API.
- Ensure all request header have this parameter ["Content-Type" : "application/json"].


Example TestCase:
-----------------------------------------------------------------------------------------------

Case 1: Primary functionalities
-----------------------------------------------------------------------------------------------

1. Cashier creates an order

	POST  http://127.0.0.1:5000/orders

	Put below json in Body:
	```json
	{
		"cost": 5.2,
		"coffeType": 0,
		"isPaied": false,
		"orderID": "201810200001",
		"isPrepared": false,
		"date": "2019-02-30",
		"payment": {
			"type": 0,
			"amount": 5.2,
			"cardNumber": "",
			"cardName":"",
			"expireDate": "18/08"
		},
		"addition": ""
	}
	```

2. Cashier amends the order by adding an addition

	PUT  http://127.0.0.1:5000/orders/201810200001

	Body:
	```json
	{
		"cost": 5.2,
		"coffeType": 0,
		"isPaied": false,
		"orderID": "201810200001",
		"isPrepared": false,
		"date": "2019-02-30",
		"payment": {
			"type": 0,
			"amount": 5.2,
			"cardNumber": "",
			"cardName":"",
			"expireDate": ""
		},
		"addition": "No milk, more sugar"
	}
	```

3. Cashier creates a Payment for the order
	
	POST  http://127.0.0.1:5000/payments/201810200001
	
	Body:
	```json
	{
	    "type": 1,
	   	"amount": 5.2,
	   	"cardNumber": "",
	   	"cardName":"",
	   	"expireDate": ""
	}
	```
	
	or pay by card
	
	```json
	{
	    "type": 2,
	   	"amount": 6.5,
	   	"cardNumber": "5643126745321234",
	   	"cardName":"Smith Lily",
	   	"expireDate": "03/20"
	}
	```

4. Barista gets the list of all Open Orders (only one order is available)

	GET http://127.0.0.1:5000/orders/open
	
	Response json list:
	```json
	
	[
		{
			"cost": 5.2,
			"coffeType": 0,
			"isPaied": true,
			"orderID": "201810200001",
			"isPrepared": false,
			"date": "2019-02-30",
			"payment": {
				"type": 1,
				"amount": 5.2,
				"cardNumber": "",
				"cardName":"",
				"expireDate": ""
			},
			"addition": "No milk, more sugar"
		}
	]
	
	```

5. Barista changes the status of the Order to being prepared

	PATCH http://127.0.0.1:5000/orders/prepare/201810200001
	
	Response:
	```
	{
		"cost": 5.2,
		"coffeType": 0,
		"isPaied": true,
		"orderID": "201810200001",
		"isPrepared": true,
		"date": "2019-02-30",
		"payment": {
			"type": 1,
			"amount": 5.2,
			"cardNumber": "",
			"cardName":"",
			"expireDate": ""
		},
		"addition": "No milk, more sugar"
	}
	```
	
6. Barista checks if the order is paid
	
	GET http://127.0.0.1:5000/payments/201810200001
	
	Response:
	```
	{
		"type": 1,
		"amount": 5.2,
		"cardNumber": "",
		"cardName":"",
		"expireDate": ""
	}
	
	```

7. Barista releases the order

	DELETE http://127.0.0.1:5000/orders/201810200001
	
	Response:
	```
	{
		"msg": "Release success"
	}
	```
	
	
8. Barista gets the list of all Open Orders (No order is available)
	
	GET http://127.0.0.1:5000/orders/open
	
	Response json list:
	```
		[]
	```



Case 2: Order cannot be amended or cancelled after payment
-----------------------------------------------------------------------------------------------

1. Cashier creates an order

	POST  http://127.0.0.1:5000/orders

	Put below json in Body:
	```json
	{
		"cost": 5.2,
		"coffeType": 0,
		"isPaied": false,
		"orderID": "201810200001",
		"isPrepared": false,
		"date": "2019-02-30",
		"payment": {
			"type": 0,
			"amount": 5.2,
			"cardNumber": "",
			"cardName":"",
			"expireDate": ""
		},
		"addition": ""
	}
	```

2. Cashier creates a Payment for the order

	POST  http://127.0.0.1:5000/payments/201810200001
		
	Body:
	```json
	{
	    "type": 1,
	   	"amount": 5.2,
	   	"cardNumber": "",
	   	"cardName":"",
	   	"expireDate": ""
	}
	```
		
	or pay by card
		
	```json
	{
	    "type": 2,
	   	"amount": 6.5,
	   	"cardNumber": "5643126745321234",
	   	"cardName":"Smith Lily",
	   	"expireDate": "03/20"
	}
	```

3. Cashier amends the order by adding an addition, but it is refused
	
	PUT  http://127.0.0.1:5000/orders/201810200001

	Body:
	```json
	{
		"cost": 5.2,
		"coffeType": 0,
		"isPaied": false,
		"orderID": "201810200001",
		"isPrepared": false,
		"date": "2019-02-30",
		"payment": {
			"type": 0,
			"amount": 5.2,
			"cardNumber": "",
			"cardName":"",
			"expireDate": ""
		},
		"addition": "No milk, more sugar"
	}
	```
	
	Response:
	```
	{
		"error": "The order is already paid, it can't be changed."
	}
	```
	
	

4. Cashier tries to cancel the order, but it is refused

	DELETE http://127.0.0.1:5000/orders/201810200001
	
	Response:
	```
	{
		"msg": "The order is already paid, it can't be cancelled."
	}
	```


Case 3: Multiple Order
-----------------------------------------------------------------------------------------------

1. Cashier creates an order (order 1)
	
	POST  http://127.0.0.1:5000/orders

	Put below json in Body:
	```json
	{
		"cost": 5.2,
		"coffeType": 0,
		"isPaied": false,
		"orderID": "201810200001",
		"isPrepared": false,
		"date": "2019-02-30",
		"payment": {
			"type": 0,
			"amount": 5.2,
			"cardNumber": "",
			"cardName":"",
			"expireDate": ""
		},
		"addition": ""
	}
	```

2. Cashier creates an order (order 2)

	POST  http://127.0.0.1:5000/orders

	Put below json in Body:
	```json
	{
		"cost": 6.5,
		"coffeType": 0,
		"isPaied": false,
		"orderID": "201810200002",
		"isPrepared": false,
		"date": "2019-02-30",
		"payment": {
			"type": 0,
			"amount": 6.5,
			"cardNumber": "",
			"cardName":"",
			"expireDate": ""
		},
		"addition": ""
	}
	```
	
3. Cashier creates an order (order 3)

	POST  http://127.0.0.1:5000/orders

	Put below json in Body:
	```json
	{
		"cost": 5.6,
		"coffeType": 0,
		"isPaied": false,
		"orderID": "201810200003",
		"isPrepared": false,
		"date": "2019-02-30",
		"payment": {
			"type": 0,
			"amount": 5.6,
			"cardNumber": "",
			"cardName":"",
			"expireDate": ""
		},
		"addition": ""
	}
	```
4. Cashier amends the Order‐2 by adding an addition

	PUT  http://127.0.0.1:5000/orders/201810200002

	Put below json in Body:
	```json
	{
		"cost": 6.5,
		"coffeType": 0,
		"isPaied": false,
		"orderID": "201810200002",
		"isPrepared": false,
		"date": "2019-02-30",
		"payment": {
			"type": 0,
			"amount": 6.5,
			"cardNumber": "",
			"cardName":"",
			"expireDate": ""
		},
		"addition": "No sugar, more milk"
	}
	```

5. Cashier creates a Payment for the order‐1

	POST	http://127.0.0.1:5000/payments/201810200001
	
	Body:
	```
	{
	    "type": 2,
	   	"amount": 5.2,
	   	"cardNumber": "5643126745321234",
	   	"cardName":"Smith Lily",
	   	"expireDate": "03/20"
	}
	```
	
6. Cashier creates a Payment for the order‐2
	
	POST	http://127.0.0.1:5000/payments/201810200002
	
	Body:
	```
	{
	    "type": 1,
	   	"amount": 6.5,
	   	"cardNumber": "",
	   	"cardName":"",
	   	"expireDate": ""
	}
	```
	
7. Barista gets the list of all Open Orders (three orders are available)

	GET		http://127.0.0.1:5000/orders/open
	
	Response:
	```
	[
		{
			"cost": 5.2,
			"coffeType": 0,
			"isPaied": true,
			"orderID": "201810200001",
			"isPrepared": false,
			"date": "2019-02-30",
			"payment": {
			    "type": 2,
			   	"amount": 5.2,
			   	"cardNumber": "5643126745321234",
			   	"cardName":"Smith Lily",
			   	"expireDate": "03/20"
			},
			"addition": ""
		},
		{
			"cost": 6.5,
			"coffeType": 0,
			"isPaied": true,
			"orderID": "201810200002",
			"isPrepared": false,
			"date": "2019-02-30",
			"payment": {
			    "type": 1,
			   	"amount": 6.5,
			   	"cardNumber": "",
			   	"cardName":"",
			   	"expireDate": ""
			},
			"addition": "No sugar, more milk"
		},
		{
			"cost": 5.6,
			"coffeType": 0,
			"isPaied": false,
			"orderID": "201810200003",
			"isPrepared": false,
			"date": "2019-02-30",
			"payment": {
				"type": 0,
				"amount": 5.6,
				"cardNumber": "",
				"cardName":"",
				"expireDate": ""
			},
			"addition": ""
		}
		
		
	]
	```
-----------------------------------------------------------------------------------------------
END
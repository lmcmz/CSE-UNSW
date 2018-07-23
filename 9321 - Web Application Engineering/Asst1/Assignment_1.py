from flask import Flask,jsonify, url_for
from flask_restful import reqparse, request
from enum import Enum, unique, IntEnum
import json

app = Flask(__name__)
database = []
release = []

def check_require(json_data, vars):
	for var in vars:
		if not var in json_data:
			return False, var
	return True, None

def check_sub_require(json_data, variable, vars):
	for var in vars:
		if not var in json_data[variable]:
			return False, var
	return True, None
	
	
def add_optional(json_data,vars):
	for var in vars:
		if not var in json_data:
			json_data[var] = ""
	return json_data

def add_sub_optional(json_data, variable, vars):
	for var in vars:
		if not var in json_data[variable]:
			json_data[variable][var] = ""
	return json_data

def check_repeat(order):
	for item in database:
		if item.orderID == order.orderID:
			return False
	return True

class PaymentType(IntEnum):
	Invailed = 0
	Cash = 1
	Card = 2

class CoffeType(IntEnum):
	Espresso = 0
	Double_Espresso = 1
	Short_Macchiato = 2
	Long_Macchiato = 3
	Ristretto = 4
	Long_Black = 5
	Latte = 6
	Cappuccino = 7
	Flat_White = 8
	Piccolo_Latte = 9
	Mocha = 10
	Affogato = 11

class Payment:
	def __init__(self, type, amount, cardNumber, cardName, expireDate, *args, **kwargs):
		self.type = type
		self.amount = amount
		self.cardNumber = cardNumber
		self.cardName = cardName
		self.expireDate = expireDate
		
	def toDict(self):
		if isinstance(self, dict):
			return self
		dict_self = self.__dict__
		return dict_self

	def toJSON(self):
			return json.dumps(self, default=lambda o: o.__dict__, 
				sort_keys=True, indent=4)

class Order(object):
	def __init__(self, orderID, cost, coffeType, payment, addition, isPaid, isPrepared, date, *args, **kwargs):
		self.orderID = orderID
		self.cost = cost
		self.coffeType = coffeType
		self.payment = Payment(payment['type'], payment['amount'], payment['cardName'], payment['cardNumber'], payment['expireDate'])
		self.addition = addition
		self.isPaid = isPaid
		self.isPrepared = isPrepared
		self.date = date
		
	def toDict(self):
		if not isinstance(self.payment, dict):
			self.payment = self.payment.__dict__
		dict_self = self.__dict__
		return dict_self
	
	def toJSON(self):
			return json.dumps(self, default=lambda o: o.__dict__, 
				sort_keys=True, indent=4)
				
@app.route("/orders", methods=['POST'])
def create_order():
	data = request.data
	json_data = json.loads(data)
	json_data = add_optional(json_data, ["addition", "payment"])
	json_data = add_sub_optional(json_data, 'payment', ['cardNumber', 'cardName', 'type', 'amount', 'expireDate'])
	
	passed, var = check_require(json_data, ["orderID", "cost", "coffeType", "isPaid", "isPrepared", "date"])	
	if not passed:
		return jsonify(error="Miss require parameters: " + str(var)),200
	
	sub_passed, sub_var = check_sub_require(json_data, "payment", ["type", "amount"])
	if not sub_passed:
		return jsonify(error="Miss require parameters: payment - " + str(var)),200
	
	try:
		order = Order(**json_data)
	except BaseException:
		return jsonify(error="Invaild data"),200
	else:
		if not check_repeat(order):
			return jsonify(error="Repeat order"),200	
		if order.isPaid:
			return jsonify(error="Order can't be paid when it was just created"),200	
		if order.isPrepared:	
			return jsonify(error="Order can't be prepared when it was just created"),200	
		database.append(order)
		return jsonify(updatePayment=url_for('create_payment', id=order.orderID), orderInfo=order.toDict()),201

@app.route("/orders", methods=['GET'])
def get_orders():
	orderList = list()
	for order in database:
		orderList.append(order.toDict())
	for reOrder in release:
		orderList.append(reOrder.toDict())
	return jsonify(orderList)

@app.route("/orders/unpaid", methods=['GET'])
def get_unpaid_orders():
	orderList = list()
	for order in database:
		if not order.isPaid:
			orderList.append(order.toDict())
	return jsonify(orderList)

@app.route("/orders/paid", methods=['GET'])
def get_paid_orders():
	orderList = list()
	for order in database:
		if order.isPaid:
			orderList.append(order.toDict())
	return jsonify(orderList)

@app.route("/orders/paid/<id>", methods=['GET'])
def check_paid_order(id):
	orderList = list()
	for order in database:
		if order.orderID == id:
			return jsonify(msg=order.isPaid)	
	return jsonify(error="The order ID is invaild"), 404

@app.route("/orders/open", methods=['GET'])
def get_open_orders():
	orderList = list()
	for order in database:
		if not order.isPrepared:
			orderList.append(order.toDict())
	return jsonify(orderList)

@app.route("/orders/release", methods=['GET'])
def get_release_orders():
	return jsonify([order.toDict() for order in release])

@app.route("/orders/<id>", methods=['GET'])
def get_order(id):
	for order in database:
		if order.orderID == id:
			return jsonify(order.toDict())
	return jsonify(error="The order ID is invaild"), 404

@app.route("/orders/<id>", methods=['DELETE'])
def cancel_order(id):
	for order in database:
		if order.orderID == id:
			if not order.isPaid:
				database.remove(order)
				return jsonify(orderID=id), 200
			else:
				return jsonify(msg="The order is already paid, it can't be cancelled."), 200				
	return jsonify(error="The order ID is invaild"), 200

@app.route("/orders/<id>", methods=['PUT'])
def update_order(id):
	data = request.data
	json_data = json.loads(data)
	json_data = add_optional(json_data, ["addition", "payment"])
	json_data = add_sub_optional(json_data, 'payment', ['cardNumber', 'cardName'])
	passed, var = check_require(json_data, ["orderID", "cost", "coffeType", "isPaid", "isPrepared", "date"])	
	if not passed:
		return jsonify(error="Miss require parameters: " + str(var)),200
	sub_passed, sub_var = check_sub_require(json_data, "payment", ["type", "amount"])
	if not sub_passed:
		return jsonify(error="Miss require parameters: payment - " + str(var)),200
	
	try:
		new_order = Order(**json_data)
	except BaseException:
		return jsonify(error="Invaild data"),200
	else:
		if not new_order.orderID == id:
			return jsonify(error="Can't update other order detail"), 200
		for order in database:
			if order.orderID == id:
				if order.isPaid:
					return jsonify(error="The order is already paid, it can't be changed."), 200
				database.remove(order)
				database.append(new_order)
				return jsonify(new_order.toDict()), 200
		return jsonify(error="The order ID is invaild"), 404
	
@app.route("/orders/prepare/<id>", methods=['PATCH'])
def prepared_order(id):
	for order in database:
		if order.orderID == id:
			order.isPrepared = True
			return jsonify(order.toDict()), 200
	return jsonify(error="The order ID is invaild"), 404
	

@app.route("/orders/release/<id>", methods=['DELETE'])
def release_order(id):
	for order in database:
		if order.orderID == id:
			if not order.isPrepared:
				return jsonify(error="The order is not already")
			if not order.isPaid:
				return jsonify(error="The order is unpaid")
			database.remove(order)
			release.append(order)
			return jsonify(msg="Release success"), 200
	return jsonify(error="The order ID is invaild"), 404

@app.route("/payments/<id>", methods=['GET'])
def get_payment(id):
	for order in database:
		if order.orderID == id:
			payment = order.payment
			if isinstance(payment, dict):
				return jsonify(payment), 200	
			return jsonify(payment.toDict()), 200
	return jsonify(error="The order ID is invaild"), 404

@app.route("/payments/<id>", methods=['POST'])
def create_payment(id):
	data = request.data
	json_data = json.loads(data)
	json_data = add_optional(json_data, ["cardName", "cardNumber"])
	passed, var = check_require(json_data, ["type", "amount"])	
	if not passed:
		return jsonify(error="Miss require parameters: " + str(var)),200
	
	try:
		new_payment = Payment(**json_data)
	except BaseException:
		return jsonify(error="The invaild data"), 404
	else:
		for order in database:
			if order.orderID == id:
				order.payment = new_payment
				order.isPaid = True
				return jsonify(new_payment.toDict()), 201
		return jsonify(error="The order ID is invaild"), 404

if __name__ == "__main__":
	app.config['JSON_AS_ASCII'] = False
	app.run()

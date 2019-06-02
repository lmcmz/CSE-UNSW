// COMP6452 Assignment 1 - Smart Contract
// Author : Hao Fu
// StudentID : z5102511
// Contact : lmcmze@gmail.com
/* ----------------------------------------------------------------- */


/* -----------------------------------------------------------------

	Contract number: 0xD742FBBE7fE3524DA60d0dE12f24b1aF3428fa1f
	https://rinkeby.etherscan.io/tx/0x70a9a0f2f6a1593556d0ebf31218eaf4a5c85a048c813a0cbfb335a39ae1ba61
	
-----------------------------------------------------------------*/

pragma solidity ^0.4.16;

contract Lunch {

	// Friends who can vote for food choice.   
	struct Voter {
		bool allowToVote; // to determinate user can vote or not
		bool voted;  // if true, that person already voted
		uint vote;   // index of the voted proposal
	}

	// Food 
	struct Food {
		bytes32 name;   // short name (up to 32 bytes)
		uint count; // number of accumulated votes
	}
	
	// Show all voters
	struct AllVoter {
		uint number;    //
		address[] name;
	}

	// The person who create this food choice
	address public creator;
	
	// If we have winner, stop voting
	bool public hadWinner;
	
	// The threshold to find winner
	uint public quorum;
	
	// The food which win in this voting
	bytes32 winner;
	
	// Stores a `Voter` struct for each possible address.
	mapping(address => Voter) voters;

	// A dynamically-sized array of `Food` structs.
	Food[] foodList;
	
	// Struct to store all voter info
	AllVoter allVoter;
	
	// Show the food list
	bytes32[] menu;

	// Create a new ballot to choose one of `foodNames`.
	function Lunch(bytes32[] foodNames) public {
		
		creator = msg.sender;
		
		// Creator can vote as well
		voters[creator].allowToVote = true;
		allVoter.name.push(creator);
		allVoter.number = allVoter.name.length;
		
		// No winner initially        
		hadWinner = false;
		
		// The threshold to win this game
		quorum = 2;

		// Add food list to food list
		for (uint i = 0; i < foodNames.length; i++) {
			foodList.push(Food({
				name: foodNames[i],
				count: 0
			}));
			// Add food list to menu
			menu.push(foodNames[i]);
		}
	}
	
	// Only creator can add new food to menu
	function addFood(bytes32 food) public {
		require(msg.sender == creator);
		foodList.push(Food({ 
			name: food,
			count: 0
		}));
		menu.push(food);
	}
	
	// Show the menu
	function showMenu() public view returns (bytes32[] menu_) {
		 menu_ = menu;
	}
	
	// Give the right of vote to user
	function addFriendToVote(address voter) public {
		
		// Only creator can add voter
		// The voter have not allow to vote
		// The voter haven't voted
		require(
			(msg.sender == creator) &&
			!voters[voter].voted &&
			(voters[voter].allowToVote == false)
		);
		
		// Allow the user to vote
		voters[voter].allowToVote = true;
		
		// Add it to voter list
		allVoter.name.push(voter);
		allVoter.number = allVoter.name.length;
	}
	
	// Show all voter in this game
	function showAllVoter() public view returns (uint number_,address[] outVote){
		number_ = allVoter.number;
		outVote = allVoter.name;
	}

	// Voter vote the food
	function vote(uint food) public {
		
		// Once we had winner, stop voting
		require(hadWinner == false);
		
		Voter storage sender = voters[msg.sender];
		// The voter must have the right to vote and not voted yet
		require(!sender.voted && sender.allowToVote == true);
		// Mark voted
		sender.voted = true;
		// Record the food he voted
		sender.vote = food;
		
		// Food count ++
		foodList[food].count += 1;
		
		// If the count reached the quorum
		if (foodList[food].count >= quorum) {
			// Mark the winner 
			hadWinner = true;
			winner = foodList[food].name;
		}
	}
	
	// Show the result of the poll
	function getResult() public view returns(bytes32 winner_) {
		winner_ = winner;
	}

	// Desrtoy this contract
	function desrtoy() public
	{ 
		if (msg.sender == creator)   // Only creator allow to do so
			suicide(creator);  // TODO: "suicide" has been deprecated
	}
}

/*
	Extenstion
	-----------------------------------------------------------------
	
	# ** Delegate **
	----
	- a voter can ask another voter to vote for him
	
	// Voter need add new attribute - tickets 
	
	 struct Voter {
		 .....
		unit tickets;
	 }
	
	// delegator - who help the voter to vote
	// success - if the process is successed, return true
	
	function delegateTo(address delegator) returns (bool success) {
		require( sender not vote yet and have the right to vote )
		give self ticket to delegator.
		set self tickets to 0.
	}
	
	function vote(uint food) public  {
		....
		voter(delegator) have use all their ticket to choice
	}
	
	# ** Random vote **
	----
	- a voter have no idea which should vote
	
	Then voter can randomly vote a food.
	
	function randomVote(unit food) public {
		require( sender not vote yet and have the right to vote )
		Random a number between our foodlist
		vote user ticket to the choice
	}
	
	# ** Byte32 -> String / String -> Byte32 **
	----
	- make our food and vote name more readable
	
	function bytes32ToString(bytes32 food) constant returns (string) {
		convert bytes32 to string
		return the string
	}
	
	function stringTobytes32(String name) constant returns (bytes32) {
		convert string to bytes32
		return the bytes32
	}
	
*/